# The linkable library for original dualtree KDE algorithm
librule(
    name = "dualtree_kde",
    sources = [],
    headers = ["bandwidth_lscv.h",
               "dataset_scaler.h",
               "dualtree_kde.h",               
               "dualtree_kde_common.h",
               "dualtree_kde_cv.h",
               "dualtree_kde_cv_common.h",
               "dualtree_kde_cv_impl.h",
               "dualtree_kde_impl.h",
               "dualtree_vkde.h",
               "dualtree_vkde_impl.h",
               "general_spacetree.h",
               "gen_metric_tree.h",
               "gen_metric_tree_impl.h",
               "inverse_normal_cdf.h",
               "kde_stat.h",
               "naive_kde.h"],
    deplibs = ["../series_expansion:series_expansion",
               "mlpack/allknn:allknn",
               "fastlib:fastlib_int"]
    )

# The linkable library for KDE using the original fast Gauss transform
librule(
    name = "fgt_kde",
    sources = [],
    headers = ["fgt_kde.h"],
    deplibs = ["fastlib:fastlib_int"]
    )

# The linkable library for KDE based on the original improved fast
# Gauss transform.
librule(
    name = "original_ifgt",
    sources = [],
    headers = ["original_ifgt.h"],
    deplibs = ["fastlib:fastlib_int"]
    )

# The linkable library for FFT-based KDE
librule(
    name = "fft_kde",                        
    sources = [],                            
    headers = ["fft_kde.h"],                 
    deplibs = ["fastlib:fastlib_int"]
    )

# The test driver for the FFT-based KDE
binrule(
    name = "fft_kde_bin",
    sources = ["fft_kde_main.cc"],
    headers = ["fft_kde.h"],
    deplibs = ["fastlib:fastlib_int"]
    )

# The test driver for the FGT-based KDE
binrule(
    name = "fgt_kde_bin",
    sources = ["fgt_kde_main.cc"],
    headers = ["fgt_kde.h"],
    deplibs = ["fastlib:fastlib_int"]
    )

# The test-driver for the original dualtree KDE
binrule(
    name = "dualtree_kde_bin",
    sources = ["dualtree_kde_main.cc"],
    headers = [],
    deplibs = [":dualtree_kde",
               "../series_expansion:series_expansion",
               "fastlib:fastlib_int"]
    )

# The test-driver for the original IFGT-based KDE
binrule(
    name = "original_ifgt_bin",
    sources = ["original_ifgt_main.cc"],              
    headers = [],                            
    deplibs = [":original_ifgt",
               "fastlib:fastlib_int"]
    )

# The driver for the bandwidth cross-validator.
binrule(
    name = "kde_cv_bin",
    sources = ["kde_bandwidth_cv_main.cc"],
    headers = [],
    deplibs = [":dualtree_kde",
               "../series_expansion:series_expansion",
               "fastlib:fastlib_int"]
    )

# to build:
# 1. make sure have environment variables set up:
#    $ source /full/path/to/fastlib/script/fl-env /full/path/to/fastlib
#    (you might want to put this in bashrc)
# 2. fl-build main
#    - this automatically will assume --mode=check, the default
#    - type fl-build --help for help
# 3. ./main
#    - to build same target again, type: make
#    - to force recompilation, type: make clean
