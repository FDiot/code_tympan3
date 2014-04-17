/#include "(.*\/)?TYPH.*\.h/ {
print "#ifdef TYMPAN_USE_PRECOMPILED_HEADER"
print
print "#endif // TYMPAN_USE_PRECOMPILED_HEADER"
next
}
{print}

