#ifndef INT_OP_GEN_INTERFACE
#define INT_OP_GEN_INTERFACE

#include <ci_bit.hxx>

#include <vector>

namespace cingulata
{
  class IIntOpGen {
  public:
    using BitVectorT = std::vector<CiBit>;

    /**
     * @brief      Add integers represented as vectors of bits. The output
     *             vector size is equal to the @c lhs input vector size
     *
     * @param[in]  lhs   The left hand side bit vector
     * @param[in]  rhs   The right hand side bit vector
     *
     * @return     The sum of input bit vectors as a bit vector
     */
    virtual BitVectorT  add     ( const BitVectorT& lhs,
                                  const BitVectorT& rhs)          const;

    virtual BitVectorT  sub     ( const BitVectorT& lhs,
                                  const BitVectorT& rhs)          const;

    virtual BitVectorT  mul     ( const BitVectorT& lhs,
                                  const BitVectorT& rhs)          const;

    virtual BitVectorT  square  ( const BitVectorT& lhs)          const;


    virtual CiBit not_equal     ( const BitVectorT& lhs,
                                  const BitVectorT& rhs)          const = 0;

    virtual CiBit equal         ( const BitVectorT& lhs,
                                  const BitVectorT& rhs)          const;

    virtual CiBit compare       ( const BitVectorT& lhs,
                                  const BitVectorT& rhs,
                                  const int carry_inp = 0,
                                  const bool signed_comp = false) const = 0;

    virtual CiBit lower         ( const BitVectorT& lhs,
                                  const BitVectorT& rhs,
                                  const bool signed_comp = false) const;

    virtual CiBit lower_equal   ( const BitVectorT& lhs,
                                  const BitVectorT& rhs,
                                  const bool signed_comp = false) const;

    virtual CiBit greater_equal ( const BitVectorT& lhs,
                                  const BitVectorT& rhs,
                                  const bool signed_comp = false) const;

    virtual CiBit greater       ( const BitVectorT& lhs,
                                  const BitVectorT& rhs,
                                  const bool signed_comp = false) const;

  };
}

#endif