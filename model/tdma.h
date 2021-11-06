/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TDMA_H
#define TDMA_H

#include <cmath>
#include <ctime>
#include <sys/time.h>
#include "ns3/attribute.h"


namespace ns3 {
	namespace lorawan {
		double delta(uint16_t n_id);
		void fmt_date_tm(tm *ltm, char * date_str, double add_sec);
		double get_toa(uint16_t n_id);
		long int ts_now();
	}

}

#endif /* TDMA_H */

