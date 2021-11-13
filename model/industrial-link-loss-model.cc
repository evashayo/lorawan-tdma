// ------------------------------------------------------------------------- //

#include "ns3/object.h"
#include "ns3/random-variable-stream.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include <cmath>
#include "ns3/industrial-link-loss-model.h"

using namespace std;

namespace ns3
{
	namespace lorawan
	{
		NS_LOG_COMPONENT_DEFINE("IndustrialLinkLossModel");
        NS_OBJECT_ENSURE_REGISTERED (IndustrialLinkLossModel);

        TypeId
        IndustrialLinkLossModel::GetTypeId (void)
        {
        static TypeId tid = TypeId ("ns3::IndustrialLinkLossModel")
            .SetParent<PropagationLossModel> ()
            .SetGroupName ("Propagation")
            .AddConstructor<IndustrialLinkLossModel> ()
            .AddAttribute ("Distance0",
                        "Beginning of the first (near) distance field",
                        DoubleValue (1.0),
                        MakeDoubleAccessor (&IndustrialLinkLossModel::m_distance0),
                        MakeDoubleChecker<double> ())
            .AddAttribute ("Distance1",
                        "Beginning of the second (middle) distance field.",
                        DoubleValue (200.0),
                        MakeDoubleAccessor (&IndustrialLinkLossModel::m_distance1),
                        MakeDoubleChecker<double> ())
            .AddAttribute ("Exponent0",
                        "The exponent for the first field.",
                        DoubleValue (1.9),
                        MakeDoubleAccessor (&IndustrialLinkLossModel::m_exponent0),
                        MakeDoubleChecker<double> ())
            .AddAttribute ("Exponent1",
                        "The exponent for the second field.",
                        DoubleValue (3.8),
                        MakeDoubleAccessor (&IndustrialLinkLossModel::m_exponent1),
                        MakeDoubleChecker<double> ())
            .AddAttribute ("ReferenceLoss",
                        "The reference loss at distance d0 (dB). (Default is Friis at 1m with 5.15 GHz)",
                        DoubleValue (46.6777),
                        MakeDoubleAccessor (&IndustrialLinkLossModel::m_referenceLoss),
                        MakeDoubleChecker<double> ())
            .AddAttribute ("LossDueToShadowing",
                        "Loss due to shadowing",
                        DoubleValue (3.80),
                        MakeDoubleAccessor (&IndustrialLinkLossModel::m_lossDueToShadowing),
                        MakeDoubleChecker<double> ());;
        return tid;

        }

        IndustrialLinkLossModel::IndustrialLinkLossModel ()
        {
        }

        double 
        IndustrialLinkLossModel::DoCalcRxPower (double txPowerDbm,
                                                            Ptr<MobilityModel> a,
                                                            Ptr<MobilityModel> b) const
        {
            double distance = a->GetDistanceFrom (b);
            NS_ASSERT (distance >= 0);

            // See doxygen comments for the formula and explanation

            double pathLossDb;

            if (distance < m_distance0)
            {
                pathLossDb = 0;
            }
            else if (distance < m_distance1)
            {
                pathLossDb = m_referenceLoss
                    + 10 * m_exponent0 * std::log10 (distance / m_distance0);
            }
            else
            {
                pathLossDb = m_referenceLoss
                    + 10 * m_exponent0 * std::log10 (m_distance1 / m_distance0)
                    + 10 * m_exponent1 * std::log10 (distance / m_distance1);
            }

            NS_LOG_DEBUG ("IndustrialLinkLossModel distance=" << distance << "m, " << "attenuation=" << pathLossDb << "dB");

            //TODONS3: 
            //1. Add coefiecient on Path Loss (SIGMA) = m_lossDueToShadowing
            //2. - Fadding (F) factor from Resulting Rx = 50dB
            //3. + G(tx)
            //4. + G(rx)

            pathLossDb = pathLossDb + m_lossDueToShadowing - 50 + 3 + 9;
            
            return txPowerDbm - pathLossDb;
        }

        int64_t
        IndustrialLinkLossModel::DoAssignStreams (int64_t stream)
        {
            return 0;
        }
    }
}