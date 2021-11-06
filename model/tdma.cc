/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "tdma.h"
#include "sys/time.h"

namespace ns3 {

	namespace lorawan {

		void fmt_date_tm(tm *ltm, char * date_str, double add_sec){
//			tm *ltm = localtime(&tm_in);
			ltm->tm_sec += add_sec;   // add 50 seconds to the time
			mktime( ltm);
			sprintf(date_str, "%04d-%02d-%02d %02d:%02d:%02d", (1900 + ltm->tm_year), ltm->tm_mon+1, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
		}

		double delta(uint16_t n_id){
		    double gt = 6/1000; //Gt 6ms (Guard time)
		    double toa = get_toa(n_id);
		    double tmp = (toa + gt) * n_id;
		    return tmp;
		}

		double get_toa(uint16_t n_id){
			double n_pre=8; //Preamble length, CRS, errors, other meta data (symbols); Specify during simulation
			int sf=7; //Affect bandwidth, trans rate, range (distance) [7..12] -> Make it random, device can pick any
			int bw=125000; //Bandwidth; 125, 250, 500 kHz; Specify during simulation
			double t_sym = (pow(2, sf))/bw; // Duration of simbol

			int pl=59; //Payload length [pre][pl] in bytes [30..50]
			int crc=1; //Cyclic redundancy check flag [1-disable or 0-enable]; Specify during simulation
			int h=0; //Header flag [1-disable or 0-enable]
			int de=0; //Data rate optimizer not enabled; [1-enabled or 0-disabled]
			double cr=1; //[1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]; Specify during simulation
			double tPreamble = (double(n_pre) + 4.25)*t_sym;
			double num = 8*pl - 4*sf + 28 + 16*crc - 20*h;
			double den = 4*(sf - 2*de);
			double payloadSymbNb = 8 + fmax(ceil(num/den)*(cr + 4), double(0));
			double tPayload = payloadSymbNb * t_sym;
			double toa = tPayload + tPreamble;//Time on Air for the packets
			return toa;
		}

		long int ts_now(){
			struct timeval tp;
			gettimeofday(&tp, NULL);
			long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
			return ms;
		}

	}

}

