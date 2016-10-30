#!/usr/bin/python

import pytest
import forksqrt
import math


# test with negativ Input (expect to fail)
@pytest.mark.xfail
def test_sqrt2_neg_val():
    forksqrt.sqrt2(-10, False)


# test with char as Input (expect to fail)
@pytest.mark.xfail
def test_sqrt2_char():
    forksqrt.sqrt2('Kohlefaser', False)


# compare function sqrt2() from forksqrt with sqrt from the math modul
def test_sqrt2():
    assert float(forksqrt.sqrt2(5, False)) == float('{:.14f}'.format(math.sqrt(5)))
