//Upgraded by: Cui Zhao

#ifndef INCLUDED_RFID_GLOBAL_VARS_H
#define INCLUDED_RFID_GLOBAL_VARS_H

#include <rfid/api.h>
#include <map>
#include <sys/time.h>

namespace gr {
  namespace rfid {

    enum STATUS               {RUNNING, TERMINATED};
    enum GEN2_LOGIC_STATUS    {SEND_QUERY, SEND_QUERY_2, SEND_QUERY_3, SEND_QUERY_4, SEND_QUERY_5, SEND_QUERY_6, SEND_QUERY_7, SEND_ACK, SEND_ACK_2, SEND_ACK_3, SEND_ACK_4, SEND_ACK_5, SEND_ACK_6, SEND_QUERY_REP, SEND_QUERY_REP_2, IDLE, SEND_RN16_CW, SEND_RN16_CW_2, SEND_RN16_CW_3, SEND_RN16_CW_4, SEND_RN16_CW_5, SEND_RN16_CW_6, SEND_RN16_CW_7, SEND_RN16_CW_8, SEND_RN16_CW_9, SEND_RN16_CW_10, SEND_CW, SEND_CW_2, SEND_CW_3, SEND_CW_4, SEND_CW_5, SEND_CW_6, SEND_CW_7, SEND_CW_8, SEND_CW_9, SEND_CW_10, SEND_CW_11, SEND_CW_12, SEND_CW_13, SEND_CW_14, SEND_CW_15, SEND_CW_16, SEND_CW_17, SEND_CW_18, SEND_CW_19, SEND_CW_20, SEND_CW_21, SEND_CW_22, SEND_CW_23, SEND_CW_24, SEND_CW_25, START, SEND_QUERY_ADJUST, SEND_NAK_QR, SEND_NAK_Q, POWER_DOWN};
    enum GATE_STATUS          {GATE_OPEN, GATE_CLOSED, GATE_SEEK_RN16, GATE_SEEK_EPC};
    enum DECODER_STATUS       {DECODER_DECODE_RN16, DECODER_DECODE_EPC};
    
    struct READER_STATS
    {
      int n_queries_sent;
      int cur_inventory_round;
      int cur_slot_number;
      int max_slot_number;
      int max_inventory_round;
      int n_epc_correct;

      std::vector<int>  unique_tags_round;
      std::map<int,int> tag_reads;

      struct timeval start, end; 
    };

    struct READER_STATE
    {
      STATUS               status;
      GEN2_LOGIC_STATUS    gen2_logic_status;
      GATE_STATUS          gate_status;
      DECODER_STATUS       decoder_status;
      READER_STATS         reader_stats;

      std::vector<float> magn_squared_samples; // used for sync
      int n_samples_to_ungate; // used by the GATE and DECODER block
    };

    // CONSTANTS (READER CONFIGURATION)

    // Fixed number of slots (2^(FIXED_Q))  
    const int FIXED_Q              = 0;

    // Termination criteria
    // const int MAX_INVENTORY_ROUND = 50;
    const int MAX_NUM_QUERIES     = 1000;     // Stop after MAX_NUM_QUERIES have been sent

    // valid values for Q
    const int Q_VALUE [16][4] =  
    {
        {0,0,0,0}, {0,0,0,1}, {0,0,1,0}, {0,0,1,1}, 
        {0,1,0,0}, {0,1,0,1}, {0,1,1,0}, {0,1,1,1}, 
        {1,0,0,0}, {1,0,0,1}, {1,0,1,0}, {1,0,1,1},
        {1,1,0,0}, {1,1,0,1}, {1,1,1,0}, {1,1,1,1}
    };  

    const bool P_DOWN = false;

    // Duration in us
    const int CW_D         = 250;     // Carrier wave
    const int P_DOWN_D     = 2000;    // power down
    const int T1_D         = 180;     // Time from Interrogator transmission to Tag response.
    const int T2_D         = 360;     // Time from Tag response to Interrogator transmission.
    const int PW_D         = 10;      // Half Tari
    const int DELIM_D      = 12;      // A preamble shall comprise a fixed-length start delimiter 12.5us +/-5%
    const int TRCAL_D      = 160;     // BLF = DR/TRCAL
    const int RTCAL_D      = 60;      // 6*PW

    const int NUM_PULSES_COMMAND = 5;        // Number of pulses to detect a reader command
    const int NUMBER_UNIQUE_TAGS = 100;      // Stop after NUMBER_UNIQUE_TAGS have been read


    // Number of bits
    const int PILOT_TONE          = 12;  // Optional
    const int TAG_PREAMBLE_BITS   = 6;   // Number of preamble bits
    const int RN16_BITS           = 17;  // Dummy bit at the end
    const int EPC_BITS            = 129;  // PC + EPC + CRC16 + Dummy = 6 + 16 + 96 + 16 + 1 = 135
    const int QUERY_LENGTH        = 22;  // Query length in bits
    
    const int T_READER_FREQ = 50e3;     // BLF = 50kHz
    const float TAG_BIT_D   = 1.0/T_READER_FREQ * pow(10,6); // Duration in us
    const int RN16_D        = (RN16_BITS + TAG_PREAMBLE_BITS) * TAG_BIT_D;
    const int EPC_D         = (EPC_BITS  + TAG_PREAMBLE_BITS) * TAG_BIT_D;
    // Query command 
    const int QUERY_CODE[4] = {1,0,0,0};
    const int M[2]          = {0,0};
    const int SEL[2]        = {0,0};
    const int SESSION[2]    = {0,0};
    const int TARGET        = 0;
    const int TREXT         = 0;
    const int DR            = 0;


    const int NAK_CODE[8]   = {1,1,0,0,0,0,0,0};

//    // ACK command
    const int ACK_CODE[2]   = {0,1};

    // QueryAdjust command
    const int QADJ_CODE[4]  = {1,0,0,1};

    // Fake Ack command      //test the processing rate of ack by making a default ackbits//
//    const int ACK_CODE[18]   = {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1};

      const gr_complex cw_tmp_zc[150] =
     {
     gr_complex(0.06, 0), gr_complex(0.021, -0.201), gr_complex(0.1212, 0.3384), gr_complex(0.2136, -0.3594), gr_complex(-0.3492, 0.1536),
     gr_complex(-0.0306, 0.3582), gr_complex(-0.2796, -0.0486), gr_complex(-0.333, -0.2292), gr_complex(-0.5112, 0.3522), gr_complex(0.2322, -0.3378),
     gr_complex(-0.1902, 0.6066), gr_complex(0.0822, 0.1566), gr_complex(0.3834, -0.0138), gr_complex(-0.399, -0.591), gr_complex(0.2364, -0.5574),
     gr_complex(0.177, -0.2226), gr_complex(0.501, -0.0378), gr_complex(-0.288, -0.2514), gr_complex(0.378, -0.1404), gr_complex(0.267, -0.1506),
     gr_complex(-0.759, 0.0918), gr_complex(-0.594, -0.6732), gr_complex(0.3228, 0.1944), gr_complex(0.1512, 0.5052), gr_complex(0.309, -0.276),
     gr_complex(-0.3402, 0.312), gr_complex(-0.045, 0.5004), gr_complex(0.6918, -0.0276), gr_complex(-0.3042, 0.1314), gr_complex(-0.0306, 0.0096),
     gr_complex(-0.4866, -0.0738), gr_complex(-0.1848, 0.2286), gr_complex(0.3114, -0.2604), gr_complex(0.2418, -0.1656), gr_complex(0.1086, -0.6396),
     gr_complex(-0.516, -0.324), gr_complex(0.2628, -0.1452), gr_complex(0.081, 0.6114), gr_complex(-0.1236, -0.4896), gr_complex(-0.1002, -0.6618),
     gr_complex(0.4206, -0.2142), gr_complex(-0.1602, -0.3288), gr_complex(0.2322, 0.0144), gr_complex(-0.219, -0.234), gr_complex(0.0828, 0.624),
     gr_complex(0.0432, -0.0198), gr_complex(0.2532, 0.066), gr_complex(-0.2454, -0.3402), gr_complex(0.0216, -0.1374), gr_complex(0.2598, 0.0468),
     gr_complex(0.3, -0.4506), gr_complex(0.414, 0.0966), gr_complex(0.4272, -0.1956), gr_complex(0.2724, 0.0714), gr_complex(0.1404, 0.2694),
     gr_complex(0.1362, -0.3786), gr_complex(-0.1104, 0.8424), gr_complex(0.4554, 0.0432), gr_complex(-0.5418, -0.144), gr_complex(0.1806, 0.1872),
     gr_complex(-0.1734, 0.375), gr_complex(-0.0228, -0.4464), gr_complex(-0.0342, -0.36), gr_complex(-0.0312, -0.3102), gr_complex(0.8436, -0.3432),
     gr_complex(-0.0492, -0.1014), gr_complex(-0.3564, -0.0474), gr_complex(-0.4842, 0.339), gr_complex(-0.5628, -0.0222), gr_complex(0.2778, -0.1026),
     gr_complex(0.2292, -0.0582), gr_complex(0.3294, -0.5472), gr_complex(-0.492, 0.1026), gr_complex(-0.4782, 0.0882), gr_complex(-0.6258, -1.089),
     gr_complex(-0.0402, 0), gr_complex(-0.6258, 1.089), gr_complex(-0.4782, -0.0882), gr_complex(-0.492, -0.1026), gr_complex(0.3294, 0.5472),
     gr_complex(0.2292, 0.0582), gr_complex(0.2778, 0.1026), gr_complex(-0.5628, 0.0222), gr_complex(-0.4842, -0.339), gr_complex(-0.3564, 0.0474),
     gr_complex(-0.0492, 0.1014), gr_complex(0.8436, 0.3432), gr_complex(-0.0312, 0.3102), gr_complex(-0.0342, 0.36), gr_complex(-0.0228, 0.4464),
     gr_complex(-0.1734, -0.375), gr_complex(0.1806, -0.1872), gr_complex(-0.5418, 0.144), gr_complex(0.4554, -0.0432), gr_complex(-0.1104, -0.8424),
     gr_complex(0.1362, 0.3786), gr_complex(0.1404, -0.2694), gr_complex(0.2724, -0.0714), gr_complex(0.4272, 0.1956), gr_complex(0.414, -0.0966),
     gr_complex(0.3, 0.4506), gr_complex(0.2598, -0.0468), gr_complex(0.0216, 0.1374), gr_complex(-0.2454, 0.3402), gr_complex(0.2532, -0.066),
     gr_complex(0.0432, 0.0198), gr_complex(0.0828, -0.624), gr_complex(-0.219, 0.234), gr_complex(0.2322, -0.0144), gr_complex(-0.1602, 0.3288),
     gr_complex(0.4206, 0.2142), gr_complex(-0.1002, 0.6618), gr_complex(-0.1236, 0.4896), gr_complex(0.081, -0.6114), gr_complex(0.2628, 0.1452),
     gr_complex(-0.516, 0.324), gr_complex(0.1086, 0.6396), gr_complex(0.2418, 0.1656), gr_complex(0.3114, 0.2604), gr_complex(-0.1848, -0.2286),
     gr_complex(-0.4866, 0.0738), gr_complex(-0.0306, -0.0096), gr_complex(-0.3042, -0.1314), gr_complex(0.6918, 0.0276), gr_complex(-0.045, -0.5004),
     gr_complex(-0.3402, -0.312), gr_complex(0.309, 0.276), gr_complex(0.1512, -0.5052), gr_complex(0.3228, -0.1944), gr_complex(-0.594, 0.6732),
     gr_complex(-0.759, -0.0918), gr_complex(0.267, 0.1506), gr_complex(0.378, 0.1404), gr_complex(-0.288, 0.2514), gr_complex(0.501, 0.0378),
     gr_complex(0.177, 0.2226), gr_complex(0.2364, 0.5574), gr_complex(-0.399, 0.591), gr_complex(0.3834, 0.0138), gr_complex(0.0822, -0.1566),
     gr_complex(-0.1902, -0.6066), gr_complex(0.2322, 0.3378), gr_complex(-0.5112, -0.3522), gr_complex(-0.333, 0.2292), gr_complex(-0.2796, 0.0486),
     gr_complex(-0.0306, -0.3582), gr_complex(-0.3492, -0.1536), gr_complex(0.2136, 0.3594), gr_complex(0.1212, -0.3384), gr_complex(0.0105, 0.1005)
     }; // Paste the result from #gen_ofdm_cw.mat#

    // 110 Increment by 1, 000 unchanged, 011 decrement by 1
    const int Q_UPDN[3][3]   = { {1,1,0}, {0,0,0}, {0,1,1} };

    // FM0 encoding preamble sequences
    const int TAG_PREAMBLE[] = {1,1,0,1,0,0,1,0,0,0,1,1};

    // Gate block parameters
    const float THRESH_FRACTION = 0.75;     
    const int WIN_SIZE_D        = 200;

    // Duration in which dc offset is estimated
    const int DC_SIZE_D         = 90;

    // Global variable
    extern READER_STATE * reader_state;
    extern void initialize_reader_state();


  } // namespace rfid
} // namespace gr

#endif /* INCLUDED_RFID_GLOBAL_VARS_H */

