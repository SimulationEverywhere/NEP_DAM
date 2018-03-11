#!/bin/bash
file1="test_load_XML_DecisionMakerBehaviour_Eg1_v2.xml"
file2="XML_DecisionMakerBehaviour_Eg1_v2.xml"
file3="XML_load_differences"

diff -u -s -w -B $file1 $file2 > $file3