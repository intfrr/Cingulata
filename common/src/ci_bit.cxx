#include "ci_bit.hxx"

#include <cassert>

using namespace std;
using namespace cingulata;


IBitExec *const CiBit::bit_exec() {
  assert(m_bit_exec);
  return m_bit_exec;
}

void CiBit::set_bit_exec(IBitExec *const p_bit_exec) {
  m_bit_exec = p_bit_exec;
}

IBitExec* CiBit::m_bit_exec = nullptr;
uint CiBit::unique_name_cnt = 0;

const CiBit CiBit::zero(0);
const CiBit CiBit::one(1);

CiBit::CiBit(const bit_plain_t pt_val_p)
:
  pt_val(pt_val_p)
{
}

CiBit::CiBit(const CiBit& other) {
  copy(other);
}

CiBit::CiBit(CiBit&& other) {
  move(other);
}

CiBit& CiBit::operator=(const CiBit& other) {
  if (this != &other)
    copy(other);
  return *this;
}

CiBit& CiBit::operator=(CiBit&& other) {
  if (this != &other)
    move(other);
  return *this;
}

std::string CiBit::get_name(const std::string& prefix) {
  if (name.empty())
    name = prefix + std::to_string(unique_name_cnt++);
  return name;
}

CiBit& CiBit::set_name(const std::string& name_p) {
  name = name_p;
  return *this;
}

CiBit& CiBit::clr_name() {
  name.clear();
  return *this;
}

CiBit::bit_plain_t CiBit::get_val() const {
  return pt_val;
}

CiBit& CiBit::set_val(const bit_plain_t pt_val_p) {
  clear_obj_handle();
  pt_val = pt_val_p & 1;
  return *this;
}

CiBit& CiBit::read() {
  obj_hdl = m_bit_exec->read(get_name("i_"));
  return *this;
}

CiBit& CiBit::read(const std::string name_p) {
  return set_name(name_p).read();
}

CiBit& CiBit::write() {
  if (is_plain())
    obj_hdl = m_bit_exec->encode(get_val());
  m_bit_exec->write(obj_hdl, get_name("o_"));
  return *this;
}

CiBit& CiBit::write(const std::string name_p) {
  return set_name(name_p).write();
}

CiBit& CiBit::encrypt() {
  obj_hdl = m_bit_exec->encrypt(get_val());
  return *this;
}

CiBit::bit_plain_t CiBit::decrypt() {
  if (not is_plain())
    set_val(m_bit_exec->decrypt(obj_hdl));
  return get_val();
}

bool CiBit::is_plain() const {
  return obj_hdl.is_empty();
}

CiBit& CiBit::op_not() {
  if (is_plain())
    pt_val = negate(pt_val);
  else
    obj_hdl = m_bit_exec->op_not(obj_hdl);
  return *this;
}

#define DEFINE_OPER_PT(NAME, FALSE_CODE, TRUE_CODE)  \
CiBit& CiBit::NAME(const bit_plain_t val) { \
  if (val == 0) { \
    FALSE_CODE; \
  } else { \
    TRUE_CODE; \
  } \
  return *this; \
}

DEFINE_OPER_PT(op_and  ,  set_val(0),           );
DEFINE_OPER_PT(op_nand ,  set_val(1), op_not()  );
DEFINE_OPER_PT(op_andny,  set_val(0), op_not()  );
DEFINE_OPER_PT(op_andyn,            , set_val(0));
DEFINE_OPER_PT(op_or   ,            , set_val(1));
DEFINE_OPER_PT(op_nor  ,  op_not()  , set_val(0));
DEFINE_OPER_PT(op_orny ,  op_not()  , set_val(1));
DEFINE_OPER_PT(op_oryn ,  set_val(1),           );
DEFINE_OPER_PT(op_xor  ,            , op_not()  );
DEFINE_OPER_PT(op_xnor ,  op_not()  ,           );

#define DEFINE_OPER(NAME, OPER_NAME_IS_PLAIN, SAME_HDL_CODE) \
CiBit& CiBit::NAME(const CiBit& rhs) { \
  if (rhs.is_plain()) { \
    NAME(rhs.get_val()); \
  } \
  else if (is_plain()) { \
    *this = CiBit(rhs).OPER_NAME_IS_PLAIN(get_val()); \
  } \
  else if (obj_hdl == rhs.obj_hdl) { \
    SAME_HDL_CODE;\
  } \
  else { \
    obj_hdl = m_bit_exec->NAME(obj_hdl, rhs.obj_hdl); \
  } \
  return *this; \
}

DEFINE_OPER(op_and  ,   op_and  ,             );
DEFINE_OPER(op_nand ,   op_nand ,   op_not()  );
DEFINE_OPER(op_andny,   op_andyn,   set_val(0));
DEFINE_OPER(op_andyn,   op_andny,   set_val(0));
DEFINE_OPER(op_or   ,   op_or   ,             );
DEFINE_OPER(op_nor  ,   op_nor  ,   op_not()  );
DEFINE_OPER(op_orny ,   op_oryn ,   set_val(1));
DEFINE_OPER(op_oryn ,   op_orny ,   set_val(1));
DEFINE_OPER(op_xor  ,   op_xor  ,   set_val(0));
DEFINE_OPER(op_xnor ,   op_xnor ,   set_val(1));


CiBit& CiBit::operator += (const CiBit& rhs) {
  return op_xor(rhs);
}

CiBit& CiBit::operator -= (const CiBit& rhs) {
  return op_xor(rhs);
}

CiBit& CiBit::operator *= (const CiBit& rhs) {
  return op_and(rhs);
}

CiBit& CiBit::operator &= (const CiBit& rhs) {
  return op_and(rhs);
}

CiBit& CiBit::operator |= (const CiBit& rhs) {
  return op_or(rhs);
}

CiBit& CiBit::operator ^= (const CiBit& rhs) {
  return op_xor(rhs);
}

void CiBit::clear_obj_handle() {
  if (not is_plain())
    obj_hdl = ObjHandle();
}

void CiBit::copy(const CiBit& other) {
  if (other.is_plain())
    set_val(other.pt_val);
  else
    obj_hdl = other.obj_hdl;
  clr_name();
}

void CiBit::move(const CiBit& other) {
  pt_val = std::move(other.pt_val);
  obj_hdl = std::move(other.obj_hdl);
  name = std::move(other.name);
}

CiBit::bit_plain_t CiBit::negate(const bit_plain_t pt_val_p) {
  return 1^pt_val_p;
}

CiBit cingulata::operator +  (CiBit lhs, const CiBit& rhs) {
  return lhs += rhs;
}

CiBit cingulata::operator -  (CiBit lhs, const CiBit& rhs) {
  return lhs -= rhs;
}

CiBit cingulata::operator *  (CiBit lhs, const CiBit& rhs) {
  return lhs *= rhs;
}

CiBit cingulata::operator ~  (CiBit lhs) {
  return lhs.op_not();
}

CiBit cingulata::operator ^  (CiBit lhs, const CiBit& rhs) {
  return lhs ^= rhs;
}

CiBit cingulata::operator &  (CiBit lhs, const CiBit& rhs) {
  return lhs &= rhs;
}

CiBit cingulata::operator |  (CiBit lhs, const CiBit& rhs) {
  return lhs |= rhs;
}

CiBit cingulata::operator !  (CiBit lhs) {
  return lhs.op_not();
}

CiBit cingulata::operator && (CiBit lhs, const CiBit& rhs) {
  return lhs &= rhs;
}

CiBit cingulata::operator || (CiBit lhs, const CiBit& rhs) {
  return lhs |= rhs;
}

CiBit cingulata::operator == (CiBit lhs, const CiBit& rhs) {
  return lhs.op_xnor(rhs);
}

CiBit cingulata::operator != (CiBit lhs, const CiBit& rhs) {
  return lhs.op_xor(rhs);
}

CiBit cingulata::operator <  (CiBit lhs, const CiBit& rhs) {
  return lhs.op_andny(rhs);
}

CiBit cingulata::operator <= (CiBit lhs, const CiBit& rhs) {
  return lhs.op_orny(rhs);
}

CiBit cingulata::operator >  (CiBit lhs, const CiBit& rhs) {
  return lhs.op_andyn(rhs);
}

CiBit cingulata::operator >= (CiBit lhs, const CiBit& rhs) {
  return lhs.op_oryn(rhs);
}
