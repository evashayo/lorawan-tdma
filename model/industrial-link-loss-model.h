#ifndef INDUSTRIAL_LINK_LOSS_MODEL_H
#define INDUSTRIAL_LINK_LOSS_MODEL_H

#include "propagation-loss-model.h"

using namespace ns3;

namespace ns3
{

  namespace lorawan
  {

    class IndustrialLinkLossModel : public PropagationLossModel
    {
    public:
    /**
    * \brief Get the type ID.
    * \return the object TypeId
    */
    static TypeId GetTypeId (void);
    IndustrialLinkLossModel ();

    // Parameters are all accessible via attributes.

    private:
    /**
    * \brief Copy constructor
    *
    * Defined and unimplemented to avoid misuse
    */
    IndustrialLinkLossModel (const IndustrialLinkLossModel&);
    /**
    * \brief Copy constructor
    *
    * Defined and unimplemented to avoid misuse
    * \returns
    */
    IndustrialLinkLossModel& operator= (const IndustrialLinkLossModel&);

    virtual double DoCalcRxPower (double txPowerDbm,
                                    Ptr<MobilityModel> a,
                                    Ptr<MobilityModel> b) const;
    virtual int64_t DoAssignStreams (int64_t stream);

    double m_distance0; //!< Beginning of the first (near) distance field
    double m_distance1; //!< Beginning of the second (middle) distance field.

    double m_exponent0; //!< The exponent for the first field.
    double m_exponent1; //!< The exponent for the second field.

    double m_referenceLoss; //!< The reference loss at distance d0 (dB).
    double m_lossDueToShadowing; //!< The reference loss at distance d0 (dB).
    };
  }
}

#endif /* INDUSTRIAL_LINK_LOSS_MODEL_H */