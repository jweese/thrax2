add_library(thrax2 alignment.cpp phrasalrule.cpp phrase.cpp tree.cpp)
target_compile_features(thrax2 PUBLIC cxx_std_17)
target_compile_options(thrax2 PRIVATE -Wall -Wextra -Werror -pedantic)
