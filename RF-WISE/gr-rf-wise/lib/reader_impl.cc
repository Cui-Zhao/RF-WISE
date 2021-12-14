//Upgraded by: Cui Zhao

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "reader_impl.h"
#include "rfid/global_vars.h"
#include <sys/time.h>

namespace gr {
  namespace rfid {

    reader::sptr
    reader::make(int sample_rate, int dac_rate)
    {
      return gnuradio::get_initial_sptr
        (new reader_impl(sample_rate,dac_rate));
    }

    /*
     * The private constructor
     */
    reader_impl::reader_impl(int sample_rate, int dac_rate)
      : gr::block("reader",
              gr::io_signature::make( 1, 1, sizeof(float)),
              gr::io_signature::make( 1, 1, sizeof(gr_complex)))
    {

      GR_LOG_INFO(d_logger, "Block initialized");

      sample_d = 1.0/dac_rate * pow(10,6);

      // Number of samples for transmitting

      n_data0_s = 2 * PW_D / sample_d;
      n_data1_s = 4 * PW_D / sample_d;
      n_pw_s    = PW_D    / sample_d;
      n_cw_s    = CW_D    / sample_d;
      n_delim_s = DELIM_D / sample_d;
      n_trcal_s = TRCAL_D / sample_d;

      GR_LOG_INFO(d_logger, "Number of samples data 0 : " << n_data0_s);
      GR_LOG_INFO(d_logger, "Number of samples data 1 : " << n_data1_s);
      GR_LOG_INFO(d_logger, "Number of samples cw : "     << n_cw_s);
      GR_LOG_INFO(d_logger, "Number of samples delim : "  << n_delim_s);
      GR_LOG_INFO(d_logger, "Number of slots : "          << std::pow(2,FIXED_Q));

      // CW waveforms of different sizes
      n_cwquery_s   = (T1_D+T2_D+RN16_D)/(10*sample_d);     //RN16,  divided by 10 for avoiding segmentation fault (25MHz)
      n_cwack_s     = (3*T1_D+T2_D+EPC_D)/(25*sample_d);    // EPC,   divided by 25 for avoiding segmentation fault (25MHz)
      n_p_down_s     = (P_DOWN_D)/sample_d;  

      p_down_zc.resize(n_p_down_s);                         // Power down samples
      cw_query_zc.resize(n_cwquery_s);                      // Sent after query/query rep
      cw_ack_zc.resize(n_cwack_s);                          // Sent after ack

      std::fill_n(cw_query_zc.begin(), cw_query_zc.size(), gr_complex(1,0));
//      std::fill_n(cw_ack_zc.begin(), cw_ack_zc.size(), gr_complex(1,0));

        for (int i = 0; i < 24; i++)
      {
        for(int j =0; j < 150; j++)
        {
        cw_ack_zc[(150 * i) + j] = cw_tmp_zc[j] + gr_complex(1,0);
        }
     }

      GR_LOG_INFO(d_logger, "Carrier wave after a query transmission in samples : "     << n_cwquery_s);
      GR_LOG_INFO(d_logger, "Carrier wave after ACK transmission in samples : "        << n_cwack_s);

      // Construct vectors (resize() default initialization is zero)
      data_0_zc.resize(n_data0_s);
      data_1_zc.resize(n_data1_s);
      cw_zc.resize(n_cw_s);
      delim_zc.resize(n_delim_s);
      rtcal_zc.resize(n_data0_s + n_data1_s);
      trcal_zc.resize(n_trcal_s);

      // Fill vectors with data
      std::fill_n(data_0_zc.begin(), data_0_zc.size()/2, gr_complex(1,0));
      std::fill_n(data_1_zc.begin(), 3*data_1_zc.size()/4, gr_complex(1,0));
      std::fill_n(cw_zc.begin(), cw_zc.size(), gr_complex(1,0));
      std::fill_n(rtcal_zc.begin(), rtcal_zc.size() - n_pw_s, gr_complex(1,0)); // RTcal
      std::fill_n(trcal_zc.begin(), trcal_zc.size() - n_pw_s, gr_complex(1,0)); // TRcal

      // create preamble
      preamble_zc.insert( preamble_zc.end(), delim_zc.begin(), delim_zc.end() );
      preamble_zc.insert( preamble_zc.end(), data_0_zc.begin(), data_0_zc.end() );
      preamble_zc.insert( preamble_zc.end(), rtcal_zc.begin(), rtcal_zc.end() );
      preamble_zc2.insert( preamble_zc2.end(), trcal_zc.begin(), trcal_zc.end() );

      // create framesync
      frame_sync_zc.insert( frame_sync_zc.end(), delim_zc.begin() , delim_zc.end() );
      frame_sync_zc.insert( frame_sync_zc.end(), data_0_zc.begin(), data_0_zc.end() );
      frame_sync_zc.insert( frame_sync_zc.end(), rtcal_zc.begin() , rtcal_zc.end() );
      
      // create query rep
      query_rep_zc.insert( query_rep_zc.end(), frame_sync_zc.begin(), frame_sync_zc.end());
      query_rep_zc.insert( query_rep_zc.end(), data_0_zc.begin(), data_0_zc.end() );
      query_rep_zc.insert( query_rep_zc.end(), data_0_zc.begin(), data_0_zc.end() );
      query_rep_zc.insert( query_rep_zc.end(), data_0_zc.begin(), data_0_zc.end() );
      query_rep_zc.insert( query_rep_zc.end(), data_0_zc.begin(), data_0_zc.end() );

      // create nak
      nak_zc.insert( nak_zc.end(), frame_sync_zc.begin(), frame_sync_zc.end());
      nak_zc.insert( nak_zc.end(), data_1_zc.begin(), data_1_zc.end() );
      nak_zc.insert( nak_zc.end(), data_1_zc.begin(), data_1_zc.end() );
      nak_zc.insert( nak_zc.end(), data_0_zc.begin(), data_0_zc.end() );
      nak_zc.insert( nak_zc.end(), data_0_zc.begin(), data_0_zc.end() );
      nak_zc.insert( nak_zc.end(), data_0_zc.begin(), data_0_zc.end() );
      nak_zc.insert( nak_zc.end(), data_0_zc.begin(), data_0_zc.end() );
      nak_zc.insert( nak_zc.end(), data_0_zc.begin(), data_0_zc.end() );
      nak_zc.insert( nak_zc.end(), data_0_zc.begin(), data_0_zc.end() );

      gen_query_bits();
      gen_query_adjust_bits();
    }

    void reader_impl::gen_query_bits()
    {
      int num_ones = 0, num_zeros = 0;

      query_bits.resize(0);
      query_bits.insert(query_bits.end(), &QUERY_CODE[0], &QUERY_CODE[4]);
      query_bits.push_back(DR);
      query_bits.insert(query_bits.end(), &M[0], &M[2]);
      query_bits.push_back(TREXT);
      query_bits.insert(query_bits.end(), &SEL[0], &SEL[2]);
      query_bits.insert(query_bits.end(), &SESSION[0], &SESSION[2]);
      query_bits.push_back(TARGET);
    
      query_bits.insert(query_bits.end(), &Q_VALUE[FIXED_Q][0], &Q_VALUE[FIXED_Q][4]);
      crc_append(query_bits);
    }

//    void reader_impl::gen_ack_bits()      //make default ack//
//    {
//      ack_bits.resize(0);
//      ack_bits.insert(ack_bits.end(), &ACK_CODE[0], &ACK_CODE[18]);
//    }

    void reader_impl::gen_ack_bits(const float * in)
    {
      ack_bits.resize(0);
      ack_bits.insert(ack_bits.end(), &ACK_CODE[0], &ACK_CODE[2]);
      ack_bits.insert(ack_bits.end(), &in[0], &in[16]);
    }
  
    void reader_impl::gen_query_adjust_bits()
    {
      query_adjust_bits.resize(0);
      query_adjust_bits.insert(query_adjust_bits.end(), &QADJ_CODE[0], &QADJ_CODE[4]);
      query_adjust_bits.insert(query_adjust_bits.end(), &SESSION[0], &SESSION[2]);
      query_adjust_bits.insert(query_adjust_bits.end(), &Q_UPDN[1][0], &Q_UPDN[1][3]);
    }


    /*
     * Virtual destructor.
     */
    reader_impl::~reader_impl()
    {

    }

    void reader_impl::print_results()
    {
      std::cout << "\n --------------------------" << std::endl;
      std::cout << "| Number of queries/queryreps sent : " << reader_state->reader_stats.n_queries_sent - 1 << std::endl;
      std::cout << "| Current Inventory round : "          << reader_state->reader_stats.cur_inventory_round << std::endl;
      std::cout << " --------------------------"           << std::endl;

      std::cout << "| Correctly decoded EPC : "  <<  reader_state->reader_stats.n_epc_correct     << std::endl;
      std::cout << "| Number of unique tags : "  <<  reader_state->reader_stats.tag_reads.size() << std::endl;

      std::map<int,int>::iterator it;

      for(it = reader_state->reader_stats.tag_reads.begin(); it != reader_state->reader_stats.tag_reads.end(); it++) 
      {
        std::cout << std::hex <<  "| Tag ID : " << it->first << "  ";
        std::cout << "Num of reads : " << std::dec << it->second << std::endl;
      }

      std::cout << " --------------------------" << std::endl;
    }

    void
    reader_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = 0;
    }

    int
    reader_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {

      const float *in = (const float *) input_items[0];
      gr_complex *out =  (gr_complex*) output_items[0];
      std::vector<gr_complex> out_message;
      int n_output;
      int consumed = 0;
      int written = 0;

      consumed = ninput_items[0];
  
      switch (reader_state->gen2_logic_status)
      {
        case START:
          GR_LOG_INFO(d_debug_logger, "START");
          std::cout<<"START"<<std::endl;
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_QUERY;
          break;

        case POWER_DOWN:
          GR_LOG_INFO(d_debug_logger, "POWER DOWN");
          memcpy(&out[written], &p_down_zc[0], sizeof(gr_complex) * p_down_zc.size() );
          written += p_down_zc.size();
          reader_state->gen2_logic_status = START;    
          break;

        case SEND_NAK_QR:
          GR_LOG_INFO(d_debug_logger, "SEND NAK");
          memcpy(&out[written], &nak_zc[0], sizeof(gr_complex) * nak_zc.size() );
          written += nak_zc.size();
          memcpy(&out[written], &cw_zc[0], sizeof(gr_complex) * cw_zc.size() );
          written+=cw_zc.size();
          reader_state->gen2_logic_status = SEND_QUERY_REP;    
          break;

        case SEND_NAK_Q:
          GR_LOG_INFO(d_debug_logger, "SEND NAK");
          memcpy(&out[written], &nak_zc[0], sizeof(gr_complex) * nak_zc.size() );
          written += nak_zc.size();
          memcpy(&out[written], &cw_zc[0], sizeof(gr_complex) * cw_zc.size() );
          written+=cw_zc.size();
          reader_state->gen2_logic_status = SEND_QUERY;    
          break;

        case SEND_QUERY:
          /*if (reader_state->reader_stats.n_queries_sent % 500 == 0)
          {
            std::cout << "Running " << std::endl;
          }*/

          GR_LOG_INFO(d_debug_logger, "QUERY");
          GR_LOG_INFO(d_debug_logger, "INVENTORY ROUND : " << reader_state->reader_stats.cur_inventory_round << " SLOT NUMBER : " << reader_state->reader_stats.cur_slot_number);

          reader_state->reader_stats.n_queries_sent +=1;  
          // Controls the other two blocks
          reader_state->decoder_status = DECODER_DECODE_RN16;
          reader_state->gate_status    = GATE_SEEK_RN16;
          memcpy(&out[written], &preamble_zc[0], sizeof(gr_complex) * preamble_zc.size() );
          written+=preamble_zc.size();
          reader_state->gen2_logic_status = SEND_QUERY_2;
        break;
        case SEND_QUERY_2:
          memcpy(&out[written], &preamble_zc2[0], sizeof(gr_complex) * preamble_zc2.size() );
          written+=preamble_zc2.size();
          reader_state->gen2_logic_status = SEND_QUERY_3;
        break;
        case SEND_QUERY_3:
          for(int i = 0; i < query_bits.size()/4; i++)
          {
            if(query_bits[i] == 1)
            {
              memcpy(&out[written], &data_1_zc[0], sizeof(gr_complex) * data_1_zc.size() );
              written+=data_1_zc.size();
            }
            else
            {
              memcpy(&out[written], &data_0_zc[0], sizeof(gr_complex) * data_0_zc.size() );
              written+=data_0_zc.size();
            }
          }
          reader_state->gen2_logic_status = SEND_QUERY_4;
        break;
        case SEND_QUERY_4:
            for(int i = query_bits.size()/4; i < query_bits.size()/2; i++)
            {
                if(query_bits[i] == 1)
                {
                memcpy(&out[written], &data_1_zc[0], sizeof(gr_complex) * data_1_zc.size() );
                 written+=data_1_zc.size();
                 }
                else
               {
                memcpy(&out[written], &data_0_zc[0], sizeof(gr_complex) * data_0_zc.size() );
                written+=data_0_zc.size();
              }
          }
          reader_state->gen2_logic_status = SEND_QUERY_5;
        break;
        case SEND_QUERY_5:
            for(int i = query_bits.size()/2; i < 3*query_bits.size()/4; i++)
            {
                if(query_bits[i] == 1)
                {
                memcpy(&out[written], &data_1_zc[0], sizeof(gr_complex) * data_1_zc.size() );
                 written+=data_1_zc.size();
                 }
                else
               {
                memcpy(&out[written], &data_0_zc[0], sizeof(gr_complex) * data_0_zc.size() );
                written+=data_0_zc.size();
              }
          }
          reader_state->gen2_logic_status = SEND_QUERY_6;
        break;
        case SEND_QUERY_6:
            for(int i = 3*query_bits.size()/4; i < query_bits.size(); i++)
            {
                if(query_bits[i] == 1)
                {
                memcpy(&out[written], &data_1_zc[0], sizeof(gr_complex) * data_1_zc.size() );
                 written+=data_1_zc.size();
                 }
                else
               {
                memcpy(&out[written], &data_0_zc[0], sizeof(gr_complex) * data_0_zc.size() );
                written+=data_0_zc.size();
              }
          }
          reader_state->gen2_logic_status = SEND_RN16_CW;
        break;

        case SEND_RN16_CW:
          GR_LOG_INFO(d_debug_logger, "SEND RN16 CW");
          memcpy(&out[written], &cw_query_zc[0], sizeof(gr_complex) * cw_query_zc.size() );
          written+=cw_query_zc.size();
          reader_state->gen2_logic_status = SEND_RN16_CW_2;
        break;
        case SEND_RN16_CW_2:
          GR_LOG_INFO(d_debug_logger, "SEND RN16 CW");
          memcpy(&out[written], &cw_query_zc[0], sizeof(gr_complex) * cw_query_zc.size() );
          written+=cw_query_zc.size();
          reader_state->gen2_logic_status = SEND_RN16_CW_3;
        break;
        case SEND_RN16_CW_3:
          GR_LOG_INFO(d_debug_logger, "SEND RN16 CW");
          memcpy(&out[written], &cw_query_zc[0], sizeof(gr_complex) * cw_query_zc.size() );
          written+=cw_query_zc.size();
          reader_state->gen2_logic_status = SEND_RN16_CW_4;
        break;
        case SEND_RN16_CW_4:
          GR_LOG_INFO(d_debug_logger, "SEND RN16 CW");
          memcpy(&out[written], &cw_query_zc[0], sizeof(gr_complex) * cw_query_zc.size() );
          written+=cw_query_zc.size();
          reader_state->gen2_logic_status = SEND_RN16_CW_5;
        break;
        case SEND_RN16_CW_5:
          GR_LOG_INFO(d_debug_logger, "SEND RN16 CW");
          memcpy(&out[written], &cw_query_zc[0], sizeof(gr_complex) * cw_query_zc.size() );
          written+=cw_query_zc.size();
          reader_state->gen2_logic_status = SEND_RN16_CW_6;
        break;
        case SEND_RN16_CW_6:
          GR_LOG_INFO(d_debug_logger, "SEND RN16 CW");
          memcpy(&out[written], &cw_query_zc[0], sizeof(gr_complex) * cw_query_zc.size() );
          written+=cw_query_zc.size();
          reader_state->gen2_logic_status = SEND_RN16_CW_7;
        break;
        case SEND_RN16_CW_7:
          GR_LOG_INFO(d_debug_logger, "SEND RN16 CW");
          memcpy(&out[written], &cw_query_zc[0], sizeof(gr_complex) * cw_query_zc.size() );
          written+=cw_query_zc.size();
          reader_state->gen2_logic_status = SEND_RN16_CW_8;
        break;
        case SEND_RN16_CW_8:
          GR_LOG_INFO(d_debug_logger, "SEND RN16 CW");
          memcpy(&out[written], &cw_query_zc[0], sizeof(gr_complex) * cw_query_zc.size() );
          written+=cw_query_zc.size();
          reader_state->gen2_logic_status = SEND_RN16_CW_9;
        break;
        case SEND_RN16_CW_9:
          GR_LOG_INFO(d_debug_logger, "SEND RN16 CW");
          memcpy(&out[written], &cw_query_zc[0], sizeof(gr_complex) * cw_query_zc.size() );
          written+=cw_query_zc.size();
          reader_state->gen2_logic_status = SEND_RN16_CW_10;
        break;
        case SEND_RN16_CW_10:
          GR_LOG_INFO(d_debug_logger, "SEND RN16 CW");
          memcpy(&out[written], &cw_query_zc[0], sizeof(gr_complex) * cw_query_zc.size() );
          written+=cw_query_zc.size();
          reader_state->gen2_logic_status = IDLE;
        break;

        case SEND_ACK:
          GR_LOG_INFO(d_debug_logger, "SEND ACK");
          if (ninput_items[0] == RN16_BITS - 1)
          {
            // Controls the other two blocks
            reader_state->decoder_status = DECODER_DECODE_EPC;
            reader_state->gate_status    = GATE_SEEK_EPC;
            gen_ack_bits(in);
//            gen_ack_bits();   generate default ackbits
            // Send FrameSync
            memcpy(&out[written], &frame_sync_zc[0], sizeof(gr_complex) * frame_sync_zc.size() );
            written += frame_sync_zc.size();
            reader_state->gen2_logic_status = SEND_ACK_2;
          }
        break;
        case SEND_ACK_2:
            for(int i = 0; i < ack_bits.size()/4; i++)
            {
              if(ack_bits[i] == 1)
              {
                memcpy(&out[written], &data_1_zc[0], sizeof(gr_complex) * data_1_zc.size() );
                written += data_1_zc.size();
              }
              else
              {
                memcpy(&out[written], &data_0_zc[0], sizeof(gr_complex) * data_0_zc.size() );
                written += data_0_zc.size();
              }
            }
          reader_state->gen2_logic_status = SEND_ACK_3;
          break;
          case SEND_ACK_3:
            for(int i = ack_bits.size()/4; i < ack_bits.size()/2; i++)
            {
              if(ack_bits[i] == 1)
              {
                memcpy(&out[written], &data_1_zc[0], sizeof(gr_complex) * data_1_zc.size() );
                written += data_1_zc.size();
              }
              else
              {
                memcpy(&out[written], &data_0_zc[0], sizeof(gr_complex) * data_0_zc.size() );
                written += data_0_zc.size();
              }
            }
          reader_state->gen2_logic_status = SEND_ACK_4;
          break;
          case SEND_ACK_4:
            for(int i = ack_bits.size()/2; i < 3*ack_bits.size()/4; i++)
            {
              if(ack_bits[i] == 1)
              {
                memcpy(&out[written], &data_1_zc[0], sizeof(gr_complex) * data_1_zc.size() );
                written += data_1_zc.size();
              }
              else
              {
                memcpy(&out[written], &data_0_zc[0], sizeof(gr_complex) * data_0_zc.size() );
                written += data_0_zc.size();
              }
            }
          reader_state->gen2_logic_status = SEND_ACK_5;
          break;
          case SEND_ACK_5:
            for(int i = 3*ack_bits.size()/4; i < ack_bits.size(); i++)
            {
              if(ack_bits[i] == 1)
              {
                memcpy(&out[written], &data_1_zc[0], sizeof(gr_complex) * data_1_zc.size() );
                written += data_1_zc.size();
              }
              else
              {
                memcpy(&out[written], &data_0_zc[0], sizeof(gr_complex) * data_0_zc.size() );
                written += data_0_zc.size();
              }
            }
          consumed = ninput_items[0];
          reader_state->gen2_logic_status = SEND_CW;
          break;


        case SEND_CW:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_2;
        break;
        case SEND_CW_2:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_3;
        break;
       case SEND_CW_3:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_4;
        break;
        case SEND_CW_4:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_5;
        break;
        case SEND_CW_5:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_6;
        break;
       case SEND_CW_6:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_7;
        break;
        case SEND_CW_7:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_8;
        break;
        case SEND_CW_8:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_9;
        break;
       case SEND_CW_9:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_10;
        break;
        case SEND_CW_10:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_11;
        break;
       case SEND_CW_11:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_12;
        break;
        case SEND_CW_12:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_13;
        break;
        case SEND_CW_13:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_14;
        break;
        case SEND_CW_14:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_15;
        break;
        case SEND_CW_15:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_16;
        break;
        case SEND_CW_16:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_17;
        break;
        case SEND_CW_17:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_18;
        break;
        case SEND_CW_18:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_19;
        break;
        case SEND_CW_19:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_20;
        break;
        case SEND_CW_20:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_21;
        break;
        case SEND_CW_21:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_22;
        break;
        case SEND_CW_22:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_23;
        break;
        case SEND_CW_23:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_24;
        break;
        case SEND_CW_24:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = SEND_CW_25;
        break;
        case SEND_CW_25:
          GR_LOG_INFO(d_debug_logger, "SEND CW");
          memcpy(&out[written], &cw_ack_zc[0], sizeof(gr_complex) * cw_ack_zc.size() );
          written += cw_ack_zc.size();
          reader_state->gen2_logic_status = IDLE;      // Return to IDLE
        break;

        case SEND_QUERY_REP:
          GR_LOG_INFO(d_debug_logger, "SEND QUERY_REP");
          GR_LOG_INFO(d_debug_logger, "INVENTORY ROUND : " << reader_state->reader_stats.cur_inventory_round << " SLOT NUMBER : " << reader_state->reader_stats.cur_slot_number);
          // Controls the other two blocks
          reader_state->decoder_status = DECODER_DECODE_RN16;
          reader_state->gate_status    = GATE_SEEK_RN16;
          reader_state->reader_stats.n_queries_sent +=1;
          memcpy(&out[written], &query_rep_zc[0], sizeof(gr_complex) * query_rep_zc.size() );
          written += query_rep_zc.size();
//
//          reader_state->gen2_logic_status = IDLE;    // Return to IDLE

          reader_state->gen2_logic_status = SEND_RN16_CW;
        break;
      
        case SEND_QUERY_ADJUST:
          GR_LOG_INFO(d_debug_logger, "SEND QUERY_ADJUST");
          // Controls the other two blocks
          reader_state->decoder_status = DECODER_DECODE_RN16;
          reader_state->gate_status    = GATE_SEEK_RN16;
          reader_state->reader_stats.n_queries_sent +=1;
          memcpy(&out[written], &frame_sync_zc[0], sizeof(gr_complex) * frame_sync_zc.size() );
          written += frame_sync_zc.size();

          for(int i = 0; i < query_adjust_bits.size(); i++)
          {
            if(query_adjust_bits[i] == 1)
            {
              memcpy(&out[written], &data_1_zc[0], sizeof(gr_complex) * data_1_zc.size() );
              written+=data_1_zc.size();
            }
            else
            {
              memcpy(&out[written], &data_0_zc[0], sizeof(gr_complex) * data_0_zc.size() );
              written+=data_0_zc.size();
            }
          }
          memcpy(&out[written], &cw_query_zc[0], sizeof(gr_complex) * cw_query_zc.size());
          written+=cw_query_zc.size();
          reader_state->gen2_logic_status = IDLE;    // Return to IDLE
          break;

        default:
          // IDLE
          break;
      }
      consume_each (consumed);
      produce(0,written);//ge
//      return  written;
      return WORK_CALLED_PRODUCE;
    }

    /* Function adapted from https://www.cgran.org/wiki/Gen2 */
    void reader_impl::crc_append(std::vector<float> & q)
    {
       int crc[] = {1,0,0,1,0};

      for(int i = 0; i < 17; i++)
      {
        int tmp[] = {0,0,0,0,0};
        tmp[4] = crc[3];
        if(crc[4] == 1)
        {
          if (q[i] == 1)
          {
            tmp[0] = 0;
            tmp[1] = crc[0];
            tmp[2] = crc[1];
            tmp[3] = crc[2];
          }
          else
          {
            tmp[0] = 1;
            tmp[1] = crc[0];
            tmp[2] = crc[1];
            if(crc[2] == 1)
            {
              tmp[3] = 0;
            }
            else
            {
              tmp[3] = 1;
            }
          }
        }
        else
        {
          if (q[i] == 1)
          {
            tmp[0] = 1;
            tmp[1] = crc[0];
            tmp[2] = crc[1];
            if(crc[2] == 1)
            {
              tmp[3] = 0;
            }
            else
            {
              tmp[3] = 1;
            }
          }
          else
          {
            tmp[0] = 0;
            tmp[1] = crc[0];
            tmp[2] = crc[1];
            tmp[3] = crc[2];
          }
        }
        memcpy(crc, tmp, 5*sizeof(float));
      }
      for (int i = 4; i >= 0; i--)
        q.push_back(crc[i]);
    }
  } /* namespace rfid */
} /* namespace gr */

