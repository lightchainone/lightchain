
#ifndef __BSL_TEST_STRING_H_
#define __BSL_TEST_STRING_H_
#include <cxxtest/TestSuite.h>
#include "Lsc_test_string.hpp"
class Lsc_test_string_main : plclic CxxTest::TestSuite{
plclic:

    void test_normal_char_Lsc_Lsc_alloc_test_ctors(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_ctors();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_c_str(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_c_str();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_empty(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_empty();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_size(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_size();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_length(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_length();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_capacity(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_capacity();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_operator_square(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_operator_square();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_clear(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_clear();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_reserve(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_reserve();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_swap(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_swap();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_operator_assign_to_null(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_operator_assign_to_null();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_operator_assign_to_cstring(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_operator_assign_to_cstring();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_operator_assign_to_string(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_operator_assign_to_string();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_operator_eq_eq(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_operator_eq_eq();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_operator_not_eq(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_operator_not_eq();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_operator_lt(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_operator_lt();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_operator_gt(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_operator_gt();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_operator_lt_eq(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_operator_lt_eq();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_operator_gt_eq(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_operator_gt_eq();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_operator_shift(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_operator_shift();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_serialization(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_serialization();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_append_str(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_append_str();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_append_slc_str(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_append_slc_str();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_append_cstr(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_append_cstr();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_append_slc_cstr(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_append_slc_cstr();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_append_n_char(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_append_n_char();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_append_range(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_append_range();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_push_back(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_push_back();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_appendf(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_appendf();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_find(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_find();
    }


    void test_normal_char_Lsc_Lsc_alloc_test_rfind(){
        return Lsc_test_string<char, Lsc::Lsc_alloc<char> >().test_rfind();
    }


    void test_normal_char_std_allocator_test_ctors(){
        return Lsc_test_string<char, std::allocator<char> >().test_ctors();
    }


    void test_normal_char_std_allocator_test_c_str(){
        return Lsc_test_string<char, std::allocator<char> >().test_c_str();
    }


    void test_normal_char_std_allocator_test_empty(){
        return Lsc_test_string<char, std::allocator<char> >().test_empty();
    }


    void test_normal_char_std_allocator_test_size(){
        return Lsc_test_string<char, std::allocator<char> >().test_size();
    }


    void test_normal_char_std_allocator_test_length(){
        return Lsc_test_string<char, std::allocator<char> >().test_length();
    }


    void test_normal_char_std_allocator_test_capacity(){
        return Lsc_test_string<char, std::allocator<char> >().test_capacity();
    }


    void test_normal_char_std_allocator_test_operator_square(){
        return Lsc_test_string<char, std::allocator<char> >().test_operator_square();
    }


    void test_normal_char_std_allocator_test_clear(){
        return Lsc_test_string<char, std::allocator<char> >().test_clear();
    }


    void test_normal_char_std_allocator_test_reserve(){
        return Lsc_test_string<char, std::allocator<char> >().test_reserve();
    }


    void test_normal_char_std_allocator_test_swap(){
        return Lsc_test_string<char, std::allocator<char> >().test_swap();
    }


    void test_normal_char_std_allocator_test_operator_assign_to_null(){
        return Lsc_test_string<char, std::allocator<char> >().test_operator_assign_to_null();
    }


    void test_normal_char_std_allocator_test_operator_assign_to_cstring(){
        return Lsc_test_string<char, std::allocator<char> >().test_operator_assign_to_cstring();
    }


    void test_normal_char_std_allocator_test_operator_assign_to_string(){
        return Lsc_test_string<char, std::allocator<char> >().test_operator_assign_to_string();
    }


    void test_normal_char_std_allocator_test_operator_eq_eq(){
        return Lsc_test_string<char, std::allocator<char> >().test_operator_eq_eq();
    }


    void test_normal_char_std_allocator_test_operator_not_eq(){
        return Lsc_test_string<char, std::allocator<char> >().test_operator_not_eq();
    }


    void test_normal_char_std_allocator_test_operator_lt(){
        return Lsc_test_string<char, std::allocator<char> >().test_operator_lt();
    }


    void test_normal_char_std_allocator_test_operator_gt(){
        return Lsc_test_string<char, std::allocator<char> >().test_operator_gt();
    }


    void test_normal_char_std_allocator_test_operator_lt_eq(){
        return Lsc_test_string<char, std::allocator<char> >().test_operator_lt_eq();
    }


    void test_normal_char_std_allocator_test_operator_gt_eq(){
        return Lsc_test_string<char, std::allocator<char> >().test_operator_gt_eq();
    }


    void test_normal_char_std_allocator_test_operator_shift(){
        return Lsc_test_string<char, std::allocator<char> >().test_operator_shift();
    }


    void test_normal_char_std_allocator_test_serialization(){
        return Lsc_test_string<char, std::allocator<char> >().test_serialization();
    }


    void test_normal_char_std_allocator_test_append_str(){
        return Lsc_test_string<char, std::allocator<char> >().test_append_str();
    }


    void test_normal_char_std_allocator_test_append_slc_str(){
        return Lsc_test_string<char, std::allocator<char> >().test_append_slc_str();
    }


    void test_normal_char_std_allocator_test_append_cstr(){
        return Lsc_test_string<char, std::allocator<char> >().test_append_cstr();
    }


    void test_normal_char_std_allocator_test_append_slc_cstr(){
        return Lsc_test_string<char, std::allocator<char> >().test_append_slc_cstr();
    }


    void test_normal_char_std_allocator_test_append_n_char(){
        return Lsc_test_string<char, std::allocator<char> >().test_append_n_char();
    }


    void test_normal_char_std_allocator_test_append_range(){
        return Lsc_test_string<char, std::allocator<char> >().test_append_range();
    }


    void test_normal_char_std_allocator_test_push_back(){
        return Lsc_test_string<char, std::allocator<char> >().test_push_back();
    }


    void test_normal_char_std_allocator_test_appendf(){
        return Lsc_test_string<char, std::allocator<char> >().test_appendf();
    }


    void test_normal_char_std_allocator_test_find(){
        return Lsc_test_string<char, std::allocator<char> >().test_find();
    }


    void test_normal_char_std_allocator_test_rfind(){
        return Lsc_test_string<char, std::allocator<char> >().test_rfind();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_ctors(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_ctors();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_c_str(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_c_str();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_empty(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_empty();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_size(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_size();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_length(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_length();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_capacity(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_capacity();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_operator_square(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_operator_square();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_clear(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_clear();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_reserve(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_reserve();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_swap(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_swap();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_operator_assign_to_null(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_operator_assign_to_null();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_operator_assign_to_cstring(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_operator_assign_to_cstring();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_operator_assign_to_string(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_operator_assign_to_string();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_operator_eq_eq(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_operator_eq_eq();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_operator_not_eq(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_operator_not_eq();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_operator_lt(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_operator_lt();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_operator_gt(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_operator_gt();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_operator_lt_eq(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_operator_lt_eq();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_operator_gt_eq(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_operator_gt_eq();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_operator_shift(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_operator_shift();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_serialization(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_serialization();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_append_str(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_append_str();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_append_slc_str(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_append_slc_str();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_append_cstr(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_append_cstr();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_append_slc_cstr(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_append_slc_cstr();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_append_n_char(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_append_n_char();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_append_range(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_append_range();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_push_back(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_push_back();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_appendf(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_appendf();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_find(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_find();
    }


    void test_normal_wchar_t_Lsc_Lsc_alloc_test_rfind(){
        return Lsc_test_string<wchar_t, Lsc::Lsc_alloc<wchar_t> >().test_rfind();
    }


    void test_normal_wchar_t_std_allocator_test_ctors(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_ctors();
    }


    void test_normal_wchar_t_std_allocator_test_c_str(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_c_str();
    }


    void test_normal_wchar_t_std_allocator_test_empty(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_empty();
    }


    void test_normal_wchar_t_std_allocator_test_size(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_size();
    }


    void test_normal_wchar_t_std_allocator_test_length(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_length();
    }


    void test_normal_wchar_t_std_allocator_test_capacity(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_capacity();
    }


    void test_normal_wchar_t_std_allocator_test_operator_square(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_operator_square();
    }


    void test_normal_wchar_t_std_allocator_test_clear(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_clear();
    }


    void test_normal_wchar_t_std_allocator_test_reserve(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_reserve();
    }


    void test_normal_wchar_t_std_allocator_test_swap(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_swap();
    }


    void test_normal_wchar_t_std_allocator_test_operator_assign_to_null(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_operator_assign_to_null();
    }


    void test_normal_wchar_t_std_allocator_test_operator_assign_to_cstring(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_operator_assign_to_cstring();
    }


    void test_normal_wchar_t_std_allocator_test_operator_assign_to_string(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_operator_assign_to_string();
    }


    void test_normal_wchar_t_std_allocator_test_operator_eq_eq(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_operator_eq_eq();
    }


    void test_normal_wchar_t_std_allocator_test_operator_not_eq(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_operator_not_eq();
    }


    void test_normal_wchar_t_std_allocator_test_operator_lt(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_operator_lt();
    }


    void test_normal_wchar_t_std_allocator_test_operator_gt(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_operator_gt();
    }


    void test_normal_wchar_t_std_allocator_test_operator_lt_eq(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_operator_lt_eq();
    }


    void test_normal_wchar_t_std_allocator_test_operator_gt_eq(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_operator_gt_eq();
    }


    void test_normal_wchar_t_std_allocator_test_operator_shift(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_operator_shift();
    }


    void test_normal_wchar_t_std_allocator_test_serialization(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_serialization();
    }


    void test_normal_wchar_t_std_allocator_test_append_str(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_append_str();
    }


    void test_normal_wchar_t_std_allocator_test_append_slc_str(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_append_slc_str();
    }


    void test_normal_wchar_t_std_allocator_test_append_cstr(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_append_cstr();
    }


    void test_normal_wchar_t_std_allocator_test_append_slc_cstr(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_append_slc_cstr();
    }


    void test_normal_wchar_t_std_allocator_test_append_n_char(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_append_n_char();
    }


    void test_normal_wchar_t_std_allocator_test_append_range(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_append_range();
    }


    void test_normal_wchar_t_std_allocator_test_push_back(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_push_back();
    }


    void test_normal_wchar_t_std_allocator_test_appendf(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_appendf();
    }


    void test_normal_wchar_t_std_allocator_test_find(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_find();
    }


    void test_normal_wchar_t_std_allocator_test_rfind(){
        return Lsc_test_string<wchar_t, std::allocator<wchar_t> >().test_rfind();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_ctors(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_ctors();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_c_str(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_c_str();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_empty(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_empty();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_size(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_size();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_length(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_length();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_capacity(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_capacity();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_operator_square(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_operator_square();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_clear(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_clear();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_reserve(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_reserve();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_swap(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_swap();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_operator_assign_to_null(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_operator_assign_to_null();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_operator_assign_to_cstring(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_operator_assign_to_cstring();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_operator_assign_to_string(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_operator_assign_to_string();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_operator_eq_eq(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_operator_eq_eq();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_operator_not_eq(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_operator_not_eq();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_operator_lt(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_operator_lt();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_operator_gt(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_operator_gt();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_operator_lt_eq(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_operator_lt_eq();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_operator_gt_eq(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_operator_gt_eq();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_operator_shift(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_operator_shift();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_serialization(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_serialization();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_append_str(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_append_str();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_append_slc_str(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_append_slc_str();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_append_cstr(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_append_cstr();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_append_slc_cstr(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_append_slc_cstr();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_append_n_char(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_append_n_char();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_append_range(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_append_range();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_push_back(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_push_back();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_appendf(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_appendf();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_find(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_find();
    }


    void test_normal_unsigned_short_Lsc_Lsc_alloc_test_rfind(){
        return Lsc_test_string<unsigned short, Lsc::Lsc_alloc<unsigned short> >().test_rfind();
    }


    void test_normal_unsigned_short_std_allocator_test_ctors(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_ctors();
    }


    void test_normal_unsigned_short_std_allocator_test_c_str(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_c_str();
    }


    void test_normal_unsigned_short_std_allocator_test_empty(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_empty();
    }


    void test_normal_unsigned_short_std_allocator_test_size(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_size();
    }


    void test_normal_unsigned_short_std_allocator_test_length(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_length();
    }


    void test_normal_unsigned_short_std_allocator_test_capacity(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_capacity();
    }


    void test_normal_unsigned_short_std_allocator_test_operator_square(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_operator_square();
    }


    void test_normal_unsigned_short_std_allocator_test_clear(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_clear();
    }


    void test_normal_unsigned_short_std_allocator_test_reserve(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_reserve();
    }


    void test_normal_unsigned_short_std_allocator_test_swap(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_swap();
    }


    void test_normal_unsigned_short_std_allocator_test_operator_assign_to_null(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_operator_assign_to_null();
    }


    void test_normal_unsigned_short_std_allocator_test_operator_assign_to_cstring(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_operator_assign_to_cstring();
    }


    void test_normal_unsigned_short_std_allocator_test_operator_assign_to_string(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_operator_assign_to_string();
    }


    void test_normal_unsigned_short_std_allocator_test_operator_eq_eq(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_operator_eq_eq();
    }


    void test_normal_unsigned_short_std_allocator_test_operator_not_eq(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_operator_not_eq();
    }


    void test_normal_unsigned_short_std_allocator_test_operator_lt(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_operator_lt();
    }


    void test_normal_unsigned_short_std_allocator_test_operator_gt(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_operator_gt();
    }


    void test_normal_unsigned_short_std_allocator_test_operator_lt_eq(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_operator_lt_eq();
    }


    void test_normal_unsigned_short_std_allocator_test_operator_gt_eq(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_operator_gt_eq();
    }


    void test_normal_unsigned_short_std_allocator_test_operator_shift(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_operator_shift();
    }


    void test_normal_unsigned_short_std_allocator_test_serialization(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_serialization();
    }


    void test_normal_unsigned_short_std_allocator_test_append_str(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_append_str();
    }


    void test_normal_unsigned_short_std_allocator_test_append_slc_str(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_append_slc_str();
    }


    void test_normal_unsigned_short_std_allocator_test_append_cstr(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_append_cstr();
    }


    void test_normal_unsigned_short_std_allocator_test_append_slc_cstr(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_append_slc_cstr();
    }


    void test_normal_unsigned_short_std_allocator_test_append_n_char(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_append_n_char();
    }


    void test_normal_unsigned_short_std_allocator_test_append_range(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_append_range();
    }


    void test_normal_unsigned_short_std_allocator_test_push_back(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_push_back();
    }


    void test_normal_unsigned_short_std_allocator_test_appendf(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_appendf();
    }


    void test_normal_unsigned_short_std_allocator_test_find(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_find();
    }


    void test_normal_unsigned_short_std_allocator_test_rfind(){
        return Lsc_test_string<unsigned short, std::allocator<unsigned short> >().test_rfind();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_ctors(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_ctors();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_c_str(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_c_str();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_empty(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_empty();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_size(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_size();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_length(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_length();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_capacity(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_capacity();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_operator_square(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_operator_square();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_clear(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_clear();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_reserve(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_reserve();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_swap(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_swap();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_operator_assign_to_null(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_operator_assign_to_null();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_operator_assign_to_cstring(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_operator_assign_to_cstring();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_operator_assign_to_string(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_operator_assign_to_string();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_operator_eq_eq(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_operator_eq_eq();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_operator_not_eq(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_operator_not_eq();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_operator_lt(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_operator_lt();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_operator_gt(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_operator_gt();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_operator_lt_eq(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_operator_lt_eq();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_operator_gt_eq(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_operator_gt_eq();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_operator_shift(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_operator_shift();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_serialization(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_serialization();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_append_str(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_append_str();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_append_slc_str(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_append_slc_str();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_append_cstr(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_append_cstr();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_append_slc_cstr(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_append_slc_cstr();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_append_n_char(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_append_n_char();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_append_range(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_append_range();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_push_back(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_push_back();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_appendf(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_appendf();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_find(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_find();
    }


    void test_normal_unsigned_int_Lsc_Lsc_alloc_test_rfind(){
        return Lsc_test_string<unsigned int, Lsc::Lsc_alloc<unsigned int> >().test_rfind();
    }


    void test_normal_unsigned_int_std_allocator_test_ctors(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_ctors();
    }


    void test_normal_unsigned_int_std_allocator_test_c_str(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_c_str();
    }


    void test_normal_unsigned_int_std_allocator_test_empty(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_empty();
    }


    void test_normal_unsigned_int_std_allocator_test_size(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_size();
    }


    void test_normal_unsigned_int_std_allocator_test_length(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_length();
    }


    void test_normal_unsigned_int_std_allocator_test_capacity(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_capacity();
    }


    void test_normal_unsigned_int_std_allocator_test_operator_square(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_operator_square();
    }


    void test_normal_unsigned_int_std_allocator_test_clear(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_clear();
    }


    void test_normal_unsigned_int_std_allocator_test_reserve(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_reserve();
    }


    void test_normal_unsigned_int_std_allocator_test_swap(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_swap();
    }


    void test_normal_unsigned_int_std_allocator_test_operator_assign_to_null(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_operator_assign_to_null();
    }


    void test_normal_unsigned_int_std_allocator_test_operator_assign_to_cstring(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_operator_assign_to_cstring();
    }


    void test_normal_unsigned_int_std_allocator_test_operator_assign_to_string(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_operator_assign_to_string();
    }


    void test_normal_unsigned_int_std_allocator_test_operator_eq_eq(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_operator_eq_eq();
    }


    void test_normal_unsigned_int_std_allocator_test_operator_not_eq(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_operator_not_eq();
    }


    void test_normal_unsigned_int_std_allocator_test_operator_lt(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_operator_lt();
    }


    void test_normal_unsigned_int_std_allocator_test_operator_gt(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_operator_gt();
    }


    void test_normal_unsigned_int_std_allocator_test_operator_lt_eq(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_operator_lt_eq();
    }


    void test_normal_unsigned_int_std_allocator_test_operator_gt_eq(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_operator_gt_eq();
    }


    void test_normal_unsigned_int_std_allocator_test_operator_shift(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_operator_shift();
    }


    void test_normal_unsigned_int_std_allocator_test_serialization(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_serialization();
    }


    void test_normal_unsigned_int_std_allocator_test_append_str(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_append_str();
    }


    void test_normal_unsigned_int_std_allocator_test_append_slc_str(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_append_slc_str();
    }


    void test_normal_unsigned_int_std_allocator_test_append_cstr(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_append_cstr();
    }


    void test_normal_unsigned_int_std_allocator_test_append_slc_cstr(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_append_slc_cstr();
    }


    void test_normal_unsigned_int_std_allocator_test_append_n_char(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_append_n_char();
    }


    void test_normal_unsigned_int_std_allocator_test_append_range(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_append_range();
    }


    void test_normal_unsigned_int_std_allocator_test_push_back(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_push_back();
    }


    void test_normal_unsigned_int_std_allocator_test_appendf(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_appendf();
    }


    void test_normal_unsigned_int_std_allocator_test_find(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_find();
    }


    void test_normal_unsigned_int_std_allocator_test_rfind(){
        return Lsc_test_string<unsigned int, std::allocator<unsigned int> >().test_rfind();
    }


};
#endif  //__BSL_TEST_STRING_H_
