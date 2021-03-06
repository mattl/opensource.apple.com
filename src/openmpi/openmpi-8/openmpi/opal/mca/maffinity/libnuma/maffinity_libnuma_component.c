/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "opal_config.h"

#include "opal/constants.h"
#include "opal/mca/maffinity/maffinity.h"
#include "maffinity_libnuma.h"

/*
 * Public string showing the maffinity ompi_libnuma component version number
 */
const char *opal_maffinity_libnuma_component_version_string =
    "OPAL libnuma maffinity MCA component version " OMPI_VERSION;

/*
 * Local function
 */
static int libnuma_open(void);

/*
 * Instantiate the public struct with all of our public information
 * and pointers to our public functions in it
 */

const opal_maffinity_base_component_1_0_0_t mca_maffinity_libnuma_component = {

    /* First, the mca_component_t struct containing meta information
       about the component itself */

    {
        /* Indicate that we are a maffinity v1.0.0 component (which also
           implies a specific MCA version) */
        
        OPAL_MAFFINITY_BASE_VERSION_1_0_0,

        /* Component name and version */

        "libnuma",
        OMPI_MAJOR_VERSION,
        OMPI_MINOR_VERSION,
        OMPI_RELEASE_VERSION,

        /* Component open and close functions */

        libnuma_open,
        NULL
    },

    /* Next the MCA v1.0.0 component meta data */

    {
        /* Whether the component is checkpointable or not */
        
        true
    },

    /* Query function */

    opal_maffinity_libnuma_component_query
};


static int libnuma_open(void)
{
    mca_base_param_reg_int(&mca_maffinity_libnuma_component.maffinityc_version,
                           "priority",
                           "Priority of the libnuma maffinity component",
                           false, false, 25, NULL);

    return OPAL_SUCCESS;
}
