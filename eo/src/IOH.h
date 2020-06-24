#ifndef _IOH_h
#define _IOH_h

// contains all headers from IOHExperimenter src code
//-----------------------------------------------------------------------------


// // BBOB common used functions
#include "IOH/ALL/coco_transformation.cpp"
#include "IOH/ALL/coco_transformation.hpp"
#include "IOH/ALL/coco_transformation_objs.hpp"
#include "IOH/ALL/coco_transformation_vars.hpp"
// #include "IOH/ALL/suite_bbob_legacy_code.hpp"


// // Templates (main classes of IOHExperimenter)
// #include "IOH/ALL/IOHprofiler_problem.h"
// #include "IOH/ALL/IOHprofiler_random.h"
// #include "IOH/ALL/IOHprofiler_observer.h"
#include "IOH/ALL/IOHprofiler_csv_logger.h"


// // BBOB functions
#include "IOH/ALL/f_sphere.hpp"
#include "IOH/ALL/f_rosenbrock.hpp"

// // IOHExperimenter functions
#include "IOH/ALL/f_one_max.hpp"
#include "IOH/ALL/f_one_max_dummy1.hpp"
#include "IOH/ALL/f_one_max_dummy2.hpp"
#include "IOH/ALL/f_one_max_epistasis.hpp"
#include "IOH/ALL/f_one_max_neutrality.hpp"
#include "IOH/ALL/f_one_max_ruggedness1.hpp"
#include "IOH/ALL/f_one_max_ruggedness2.hpp"
#include "IOH/ALL/f_one_max_ruggedness3.hpp"
#include "IOH/ALL/f_w_model_one_max.hpp"



// Not sorted yet
// #include <IOH/ALL/IOHprofiler_suite.hpp>
#include <IOH/ALL/IOHprofiler_ecdf_logger.h>
#include "IOH/ALL/eoEvalIOH.h"

// #include "IOH/ALL/IOHprofiler_observer.h"
// #include "IOH/ALL/IOHprofiler_common.h"
#include "IOH/ALL/IOHprofiler_common.cpp"
#include "IOH/ALL/IOHprofiler_transformation.cpp"
#include "IOH/ALL/IOHprofiler_random.cpp"




#endif
