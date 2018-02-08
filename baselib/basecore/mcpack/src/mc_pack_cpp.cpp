#include "mc_pack_def.h"
#include "mc_pack_bits/mc_pack_cpp.h"

namespace mcpack {

int Mcpack::put_null(const char *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::is_null(const char *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::is_null_index(size_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_string_from_item(const mc_pack_item_t * , char ** ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_binary_from_item(const mc_pack_item_t * , char ** , u_int * ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_bool_from_item(const mc_pack_item_t * , bool * ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::put_bool(const char *  , bool   ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_bool(const char *  , bool *   ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_bool_index(size_t , bool *   ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_bool(const char *  , bool   ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_bool_index(size_t , bool   ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::rm(const char * , int ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::rm_index(size_t , int ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_int32_from_item(const mc_pack_item_t * , mc_int32_t *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_uint32_from_item(const mc_pack_item_t * , mc_uint32_t *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_int64_from_item(const mc_pack_item_t * , mc_int64_t *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_uint64_from_item(const mc_pack_item_t * , mc_uint64_t *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_float_from_item(const mc_pack_item_t * , float *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_dolcle_from_item(const mc_pack_item_t * , dolcle *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::put_string(const char * , const char *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::put_nstring(const char * , const char *, unsigned int) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_string(const char * , const char ** ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_string_index(size_t , const char ** ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::put_raw(const char * , const void *, u_int) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_raw(const char * , const void **, u_int * ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_raw_index(size_t , const void ** , u_int * ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::put_dolcle(const char *, dolcle ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_dolcle(const char *, dolcle * ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_dolcle_index(size_t , dolcle * ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_dolcle(const char *, dolcle ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_dolcle_index(size_t , dolcle ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::put_float(const char *, float ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_float(const char *, float *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_float_index(size_t , float *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_float(const char *, float ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_float_index(size_t , float ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::put_int32(const char * , mc_int32_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_int32(const char * , mc_int32_t *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_int32_index(size_t , mc_int32_t *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_int32(char const * , mc_int32_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_int32_index(size_t , mc_int32_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::put_uint32(const char * , mc_uint32_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_uint32(const char * , mc_uint32_t *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_uint32_index(size_t , mc_uint32_t *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_uint32 (char const * , mc_uint32_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_uint32_index(size_t , mc_uint32_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::put_int64(const char * , mc_int64_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_int64(const char * , mc_int64_t *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_int64_index(size_t , mc_int64_t *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_int64(char const * , mc_int64_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_int64_index(size_t , mc_int64_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::put_uint64(const char * , mc_uint64_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_uint64(const char * , mc_uint64_t *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::get_uint64_index(size_t , mc_uint64_t *) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_uint64(char const * , mc_uint64_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

int Mcpack::mod_uint64_index(size_t , mc_uint64_t ) {
    return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
}

}  


















