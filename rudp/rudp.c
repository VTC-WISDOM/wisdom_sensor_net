#include "rudp.h"
#include "pico/rand.h"
#include "string.h"

bool rfm69_rudp_init(
		Rfm69 *rfm,
		spi_inst_t *spi,
		uint pin_miso,
		uint pin_mosi,
		uint pin_cs,
		uint pin_sck,
		uint pin_rst,
		uint pin_irq_0,
		uint pin_irq_1
) 
{
	if (!rfm69_init(
			rfm,
			spi,
			pin_miso,
			pin_mosi,
			pin_cs,
			pin_sck,
			pin_rst,
			pin_irq_0,
			pin_irq_1
	)) return false;

	if (!rfm69_packet_format_set(rfm, RFM69_PACKET_VARIABLE))	
		return false;

	return true;
}

bool rfm69_rudp_transmit(
        Rfm69 *rfm, 
		TrxReport *report,
        uint8_t address,
        uint8_t *payload, 
        uint payload_size,
        uint timeout,
        uint8_t retries
)
{
    // Cache previous op mode so it can be restored
    // after transmit.
    uint8_t previous_mode;
    rfm69_mode_get(rfm, &previous_mode);

    rfm69_mode_set(rfm, RFM69_OP_MODE_STDBY);

	uint8_t seq_num = get_rand_32() % SEQ_NUM_RAND_LIMIT;

    uint8_t size_bytes[sizeof(payload_size)];
    for (int i = 0; i < sizeof(payload_size); i++)
        size_bytes[i] = (payload_size >> (((sizeof(payload_size) - 1) * 8) - (i * 8))) & 0xFF;

    // Get our tx_address;
    uint8_t tx_address;
    rfm69_node_address_get(rfm, &tx_address);

	// Build header
	uint8_t header[HEADER_SIZE];
	header[HEADER_PACKET_SIZE] = HEADER_EFFECTIVE_SIZE + sizeof(payload_size);
	header[HEADER_RX_ADDRESS]  = address;
	header[HEADER_TX_ADDRESS]  = tx_address;
	header[HEADER_FLAGS]       = HEADER_FLAG_RBT;
	header[HEADER_SEQ_NUMBER]  = seq_num;

	// This count does not include the RBT packet
	uint8_t num_packets = payload_size/PAYLOAD_MAX;
    if (payload_size % PAYLOAD_MAX) num_packets++;

	if (report) {
		// zero report struct
		memset(report, 0x00, (sizeof *report));
		report->tx_address = tx_address;
		report->rx_address = address;
		report->payload_size = payload_size;
		report->return_status = RUDP_TIMEOUT;
	}

    // This payload is too large and should be fplit into multiple transmissions
    if (num_packets > TX_PACKETS_MAX) {
        report->return_status = RUDP_PAYLOAD_OVERFLOW; 
        return false;
    }

    // Buffer for receiving ACK/RACK
    // Max possible size for ACK/RACK packets
    uint8_t ack_packet[HEADER_SIZE + num_packets];
    bool success = false;
    bool ack_received = false;
    for (uint retry = 0; retry <= retries; retry++) {
        
        rfm69_mode_set(rfm, RFM69_OP_MODE_STDBY);

        // Write header to fifo
        rfm69_write(
                rfm,
                RFM69_REG_FIFO,
                header,	
                HEADER_SIZE
        );

        // Write payload size as payload
        rfm69_write(
                rfm,
                RFM69_REG_FIFO,
                size_bytes,	
                sizeof(payload_size)
        );
        
        rfm69_mode_set(rfm, RFM69_OP_MODE_TX);
        _rudp_block_until_packet_sent(rfm);

        // I emply "backoff" where the timout increases with each retry plus "jitter"
        // This allows you to have a quick retry followed by successively slower retries
        // with some random deviation to avoid a certain class of timing bugs
        uint next_timeout = timeout + (retry * timeout) + (get_rand_32() % 100);
        // Retry if ACK was not received within timeout
        if (_rudp_rx_ack(rfm, seq_num + 1, next_timeout) == RUDP_TIMEOUT) continue;

        // Ack received
        ack_received = true;
        break;
    }
    if (!ack_received) goto CLEANUP; // Do not pass go

    seq_num += 2; // Set to first data packet seq num

    uint8_t seq_num_max = seq_num + num_packets - 1;

    uint8_t size;
    uint8_t offset;
    for (int i = 0; i < num_packets; i++) {
        rfm69_mode_set(rfm, RFM69_OP_MODE_STDBY);

        // Not graceful
        if (seq_num + i == seq_num_max && payload_size % PAYLOAD_MAX)
            size = payload_size % PAYLOAD_MAX;
        else
            size = PAYLOAD_MAX;

        header[HEADER_PACKET_SIZE] = HEADER_EFFECTIVE_SIZE + size;
        header[HEADER_FLAGS]       = HEADER_FLAG_DATA;
        header[HEADER_SEQ_NUMBER]  = seq_num + i;

        uint offset = PAYLOAD_MAX * i;

        // Write header to fifo
        rfm69_write(
                rfm,
                RFM69_REG_FIFO,
                header,	
                HEADER_SIZE
        );

        // Write slice of payload
        rfm69_write(
                rfm,
                RFM69_REG_FIFO,
                &payload[offset],
                size
        );

        sleep_ms(TX_INTER_PACKET_DELAY);
        rfm69_mode_set(rfm, RFM69_OP_MODE_TX);
        _rudp_block_until_packet_sent(rfm);

        if (report) report->data_packets_sent++;
    }

    uint8_t message_size = num_packets;
    uint8_t packet_num;
    uint8_t is_ok;
    bool rack_timeout;
    for (;;) {

        is_ok = false;
        rack_timeout = true;
        while (retries) {
            retries--;
            if (_rudp_rx_rack(rfm, seq_num_max, timeout, ack_packet) == RUDP_TIMEOUT) {
                rfm69_mode_set(rfm, RFM69_OP_MODE_STDBY);
                
                header[HEADER_PACKET_SIZE] = HEADER_EFFECTIVE_SIZE; 
                header[HEADER_FLAGS]       = HEADER_FLAG_DATA | HEADER_FLAG_RACK;
                header[HEADER_SEQ_NUMBER]  = seq_num;

                rfm69_write(
                        rfm,
                        RFM69_REG_FIFO,
                        header,
                        HEADER_SIZE
                );

                sleep_ms(TX_INTER_PACKET_DELAY);
                rfm69_mode_set(rfm, RFM69_OP_MODE_TX);
                _rudp_block_until_packet_sent(rfm);

                if (report) report->rack_requests_sent++;

                continue;
            }
            is_ok = ack_packet[HEADER_FLAGS] & HEADER_FLAG_OK;
            rack_timeout = false;
            break;
        }
        if (is_ok || rack_timeout) break;
        
        if (report) report->racks_received++;

        message_size = ack_packet[HEADER_PACKET_SIZE] - HEADER_EFFECTIVE_SIZE; 
        for (int i = 0; i < message_size; i++) {
            rfm69_mode_set(rfm, RFM69_OP_MODE_STDBY);

            packet_num = ack_packet[PAYLOAD_BEGIN + i]; 

            // Not graceful still
            if (packet_num == seq_num_max && payload_size % PAYLOAD_MAX)
                size = payload_size % PAYLOAD_MAX;
            else
                size = PAYLOAD_MAX;

            header[HEADER_PACKET_SIZE] = HEADER_EFFECTIVE_SIZE + size;
            header[HEADER_SEQ_NUMBER]  = packet_num;

            uint offset = PAYLOAD_MAX * (packet_num - seq_num);

            // Write header to fifo
            rfm69_write(
                    rfm,
                    RFM69_REG_FIFO,
                    header,	
                    HEADER_SIZE
            );

            // Write slice of payload
            rfm69_write(
                    rfm,
                    RFM69_REG_FIFO,
                    &payload[offset],
                    size
            );

            sleep_ms(TX_INTER_PACKET_DELAY);
            rfm69_mode_set(rfm, RFM69_OP_MODE_TX);

            _rudp_block_until_packet_sent(rfm);
			
			if (report) {
				report->data_packets_retransmitted++;
				report->data_packets_sent++;
			}
        }
    }

    if (report) {
        if (is_ok) report->return_status = RUDP_OK;
        else report->return_status = RUDP_OK_UNCONFIRMED;
    }

    success = true;
CLEANUP:
    rfm69_mode_set(rfm, previous_mode);
    return success;
}


bool rfm69_rudp_receive(
        Rfm69 *rfm, 
        TrxReport *report,
		uint8_t *address,
        uint8_t *payload, 
        uint *payload_size,
        uint per_packet_timeout,
        uint timeout
)
{
    // Cache previous op mode so it can be restored
    // after RX
    uint8_t previous_mode;
    rfm69_mode_get(rfm, &previous_mode);

    uint payload_buffer_size = *payload_size;
    *payload_size = 0;

    uint8_t rx_address;
    rfm69_node_address_get(rfm, &rx_address);

    // Max size packet buffer
    uint8_t packet[RFM69_FIFO_SIZE];
    // Header buffer
    uint8_t header[HEADER_SIZE];

    uint8_t is_rbt;
    uint8_t seq_num;

    if (report) {
		// Zero that stuff meow
		memset(report, 0x00, (sizeof *report));
        report->rx_address = rx_address;
        report->return_status = RUDP_TIMEOUT;
    }

    bool success = false;

    absolute_time_t timeout_time = make_timeout_time_ms(timeout);
    uint8_t tx_started;
RESTART_RBT_LOOP: // This is to return to the RBT loop in case of a false
                  // start receiving the transmission
    tx_started = false;
    for (;;) {
        if (get_absolute_time() >= timeout_time) break;

        rfm69_mode_set(rfm, RFM69_OP_MODE_RX);

        if (!_rudp_is_payload_ready(rfm)) {
            sleep_us(1);
            continue;
        }

        rfm69_mode_set(rfm, RFM69_OP_MODE_STDBY);

        rfm69_read(
                rfm,
                RFM69_REG_FIFO,
                packet,
                HEADER_SIZE
        );

        is_rbt = packet[HEADER_FLAGS] & HEADER_FLAG_RBT;

        if (!is_rbt) {
            // Empty the FIFO
            rfm69_read(
                    rfm,
                    RFM69_REG_FIFO,
                    packet,
                    packet[HEADER_PACKET_SIZE] - HEADER_EFFECTIVE_SIZE
            );
            continue;
        } 

        // Read expected payload size
        uint8_t size_bytes[sizeof(*payload_size)];
        rfm69_read(
                rfm,
                RFM69_REG_FIFO,
                size_bytes,
                sizeof(*payload_size) 
        );

        for (int i = 0; i < sizeof(*payload_size); i++) 
            *payload_size |= size_bytes[i] << (((sizeof(payload_size) - 1) * 8) - (i * 8));


        // Get the sender's node address
        *address = packet[HEADER_TX_ADDRESS];

        
        // Increment the sequence
        seq_num = packet[HEADER_SEQ_NUMBER] + 1;

        // Build ACK packet header
        header[HEADER_PACKET_SIZE] = HEADER_EFFECTIVE_SIZE;
        header[HEADER_RX_ADDRESS]  = *address;
        header[HEADER_TX_ADDRESS]  = rx_address;
        header[HEADER_FLAGS]       = HEADER_FLAG_RBT | HEADER_FLAG_ACK;
        header[HEADER_SEQ_NUMBER]  = seq_num;

        rfm69_write( 
                rfm,
                RFM69_REG_FIFO,
                header,	
                HEADER_SIZE
        );

        rfm69_mode_set(rfm, RFM69_OP_MODE_TX);
        _rudp_block_until_packet_sent(rfm);

        if (report) {
            report->payload_size = *payload_size;
            report->tx_address = *address;
            report->acks_sent++;
        } 

        tx_started = true;
        break;
    }
    // If we have broken from the loop but tx hasn't started,
    // we have timed out
    if (!tx_started) goto CLEANUP;


	uint8_t num_packets_expected = *payload_size/PAYLOAD_MAX;
    if (*payload_size % PAYLOAD_MAX) num_packets_expected++;

    // We have our first data packet waiting in the FIFO now
    // Set our data packet seq num bounds
    uint8_t seq_num_max = seq_num + num_packets_expected;
    seq_num++;

    bool packets_received[TX_PACKETS_MAX] = {false}; // Keep track of what packets we have received
    uint8_t num_packets_missing = num_packets_expected;

    uint payload_bytes_received = 0; 

    uint8_t is_data;
    uint8_t packet_num;
    uint8_t is_req_rack;

    absolute_time_t rack_timeout = make_timeout_time_us(per_packet_timeout * num_packets_missing);
    absolute_time_t now;
    while (num_packets_missing) {
        now = get_absolute_time();
        if (now >= timeout_time) goto CLEANUP;

        if (now >= rack_timeout) {
            rfm69_mode_set(rfm, RFM69_OP_MODE_STDBY);
            // Time to send a RACK
            uint8_t size = (num_packets_missing > PAYLOAD_MAX) ? PAYLOAD_MAX : num_packets_missing;

            header[HEADER_PACKET_SIZE] = HEADER_EFFECTIVE_SIZE + size;
            header[HEADER_FLAGS] = HEADER_FLAG_RACK;
            header[HEADER_SEQ_NUMBER] = seq_num_max;

            bool state;
            rfm69_irq2_flag_state(rfm, RFM69_IRQ2_FLAG_FIFO_NOT_EMPTY, &state);

            rfm69_write( 
                    rfm,
                    RFM69_REG_FIFO,
                    header,	
                    HEADER_SIZE
            );

            // We are actually limited by packet size how many
            // missing packets we can report. Hopefully we aren't losing
            // 61+ packets in a single TX, but worst case scenario is
            // we have to send another RACK later
            uint8_t missing_packet;
            for (int i = 0; size; i++) {
                if (packets_received[i]) continue;
                missing_packet = i + seq_num;
                rfm69_write( 
                        rfm,
                        RFM69_REG_FIFO,
                        &missing_packet,	
                        1
                );
                size--;
            }

            rfm69_mode_set(rfm, RFM69_OP_MODE_TX);

            rack_timeout = make_timeout_time_us(per_packet_timeout * num_packets_missing);
            _rudp_block_until_packet_sent(rfm);

            if (report) report->racks_sent++;
        }

        // Make sure packet is sent before leaving TX
        rfm69_mode_set(rfm, RFM69_OP_MODE_RX);
        
        if (!_rudp_is_payload_ready(rfm)) {
            sleep_us(1);
            continue;
        }

        rfm69_read(
                rfm,
                RFM69_REG_FIFO,
                packet,
                HEADER_SIZE
        );

        uint message_size = packet[HEADER_PACKET_SIZE] - HEADER_EFFECTIVE_SIZE;
        rfm69_read(
            rfm,
            RFM69_REG_FIFO,
            &packet[PAYLOAD_BEGIN],
            message_size
        );


        if (*address != packet[HEADER_TX_ADDRESS]) continue;

        is_rbt = packet[HEADER_FLAGS] & HEADER_FLAG_RBT;
        if (is_rbt) goto RESTART_RBT_LOOP;

        is_data = packet[HEADER_FLAGS] & HEADER_FLAG_DATA;
        if (!is_data) continue;

        packet_num = packet[HEADER_SEQ_NUMBER];
        if (packet_num < seq_num || packet_num > seq_num_max) continue;

        // Check if this is a request Rack
        is_req_rack = packet[HEADER_FLAGS] & HEADER_FLAG_RACK;
        if (is_req_rack && packet_num == seq_num) {
            if (report) report->rack_requests_received++;
            rack_timeout = 0;
            continue;
        }

        // Account for packet only if it is a new packet
        if (packets_received[packet_num - seq_num]) continue;
        packets_received[packet_num - seq_num] = true;

        num_packets_missing--;

        payload_bytes_received += message_size;
        if (report) {
            report->data_packets_received++;
            report->bytes_received = payload_bytes_received;
        }
        if (payload_bytes_received > payload_buffer_size) {
            if (report) report->return_status = RUDP_BUFFER_OVERFLOW;
            goto CLEANUP;
        }


        // Copy the payload data into the payload buffer
        uint payload_offset = PAYLOAD_MAX * (packet_num - seq_num);
        for (int i = 0; i < message_size; i++) {
            payload[payload_offset + i] = packet[PAYLOAD_BEGIN + i];    
        }
    }

    rfm69_mode_set(rfm, RFM69_OP_MODE_STDBY);
    header[HEADER_PACKET_SIZE] = HEADER_EFFECTIVE_SIZE;
    header[HEADER_FLAGS] = HEADER_FLAG_RACK | HEADER_FLAG_OK;
    header[HEADER_SEQ_NUMBER] = seq_num_max;

    // Send a non-guaranteed success packet
    rfm69_write(
            rfm,
            RFM69_REG_FIFO,
            header,
            HEADER_SIZE
    );

    sleep_ms(TX_INTER_PACKET_DELAY);
    rfm69_mode_set(rfm, RFM69_OP_MODE_TX);
    _rudp_block_until_packet_sent(rfm);

    if (report) report->return_status = RUDP_OK;
    success = true;

CLEANUP:
    *payload_size = payload_bytes_received;
    rfm69_mode_set(rfm, previous_mode);
    return success;
}

static RUDP_RETURN _rudp_rx_rack(
        Rfm69 *rfm,
        uint8_t seq_num,
        uint timeout,
        uint8_t *packet
)
{
    RUDP_RETURN rval = RUDP_TIMEOUT;
    uint8_t is_rack;
    bool is_seq;
    uint8_t is_ok;

    rfm69_mode_set(rfm, RFM69_OP_MODE_RX);

    absolute_time_t timeout_time = make_timeout_time_ms(timeout);
    for (;;) {
        if (get_absolute_time() > timeout_time) break;

        if (!_rudp_is_payload_ready(rfm)) {
            continue;
        }

        rfm69_read(
                rfm,
                RFM69_REG_FIFO,
                packet,
                HEADER_SIZE
        );

        uint8_t message_size = packet[HEADER_PACKET_SIZE] - HEADER_EFFECTIVE_SIZE; 
        rfm69_read(
                rfm,
                RFM69_REG_FIFO,
                &packet[PAYLOAD_BEGIN],
                message_size 
        );

        // This is a RACK packet, which is what we wanted
        is_rack = (packet[HEADER_FLAGS] & HEADER_FLAG_RACK);
        // is it the correct sequence num?
        is_seq = packet[HEADER_SEQ_NUMBER] == seq_num;

        if (!is_rack || !is_seq) continue;

        rval = RUDP_OK; 
        break;
    }
    return rval;
}

static RUDP_RETURN _rudp_rx_ack(
        Rfm69 *rfm,
        uint8_t seq_num,
        uint timeout
)
{
    RUDP_RETURN rval = RUDP_TIMEOUT;
    uint8_t packet[HEADER_SIZE];
    bool state;
    bool is_ack;
    bool is_seq;

    rfm69_mode_set(rfm, RFM69_OP_MODE_RX);

    absolute_time_t timeout_time = make_timeout_time_ms(timeout);
    for (;;) {
        if (get_absolute_time() > timeout_time) break;

        if (!_rudp_is_payload_ready(rfm)) {
            continue;
        }
        // No need to check length byte, an ack packet is only a header
        // with some flags set
        rfm69_read(
                rfm,
                RFM69_REG_FIFO,
                packet,
                HEADER_SIZE
        );

        // This is an RBT/ACK packet, which is what we wanted
        is_ack = (packet[HEADER_FLAGS] & (HEADER_FLAG_ACK | HEADER_FLAG_RBT)) > 0;
        // is it the correct sequence num?
        is_seq = packet[HEADER_SEQ_NUMBER] == seq_num;
        if (!is_ack || !is_seq) continue;

        // ACK RECEIVED
        rval = RUDP_OK; 
        break;
    }
    return rval;
}

static inline bool _rudp_is_payload_ready(Rfm69 *rfm) {
    bool state;
    rfm69_irq2_flag_state(rfm, RFM69_IRQ2_FLAG_PAYLOAD_READY, &state);
    return state;
}

static inline void _rudp_block_until_packet_sent(Rfm69 *rfm) {
    bool state = false;
    while (!state) {
        rfm69_irq2_flag_state(rfm, RFM69_IRQ2_FLAG_PACKET_SENT, &state);
        sleep_us(1);
    }
}
