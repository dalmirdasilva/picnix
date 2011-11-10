/**
 * RS (Resource System)
 * 
 * 
 * @author  Dalmir da Silva <dalmirdasilva@gmail.com>
 */
 
 #ifndef TYPES_BASIC_H
    #error Basic types requided (byte_t, bool_t)
#endif

#define VIRTUAL_ENVIROMENT                                              1

#ifndef RS_H
#define RS_H

#define RS_NULL_RESOURCE_CODE                                           0xFF 
#define RS_NULL_CLUSTER                                                 0xFF 

#define RS_NULL_RESORCE_DESCRIPTOR_ADDRESS                              0xFF
#define RS_NULL_CLUSTER_ADDRESS                                         0x00

#define RS_SIZEOF_RS                                                    24

#define RS_SIZEOF_RESOURCE_SIZE                                         2

#define RS_INEXISTENT_CLUSTER                                           0xFF

#define CLUSTER_ADDRESS_TO_NEXT(CLUSTER_ADDRESS)                        ((CLUSTER_ADDRESS) + 0)
#define CLUSTER_ADDRESS_TO_PREV(CLUSTER_ADDRESS)                        ((CLUSTER_ADDRESS) + 1)
#define CLUSTER_ADDRESS_TO_DATA(CLUSTER_ADDRESS)                        ((CLUSTER_ADDRESS) + 2)

#define RD_ADDRESS_TO_SIZE_LOW(RD_ADDRESS)                              ((RD_ADDRESS) + 0)
#define RD_ADDRESS_TO_SIZE_HIGH(RD_ADDRESS)                             ((RD_ADDRESS) + 1)
#define RD_ADDRESS_TO_FIRST_CLUSTER(RD_ADDRESS)                         ((RD_ADDRESS) + 2)
#define RD_ADDRESS_TO_FLAG(RD_ADDRESS)                                  ((RD_ADDRESS) + 3)

typedef int_t rs_resource_descriptor_t;
typedef int_t rs_cluster_t;
typedef long_int_t rs_resource_size_t;
typedef long_int_t rs_memory_address_t;
typedef int_t rs_resource_code_t;
typedef long_int_t rs_seek_int_t;

// Drivers
typedef enum {
    RS_DRIVER_VIRTUAL = 0,
    RS_DRIVER_SELF_EEPROM = 1,
    RS_DRIVER_MULTI_EXTERNAL_EEPROM = 2
} rs_driver_t;

// Resource fag bit values
typedef enum {
    RS_RESOURCE_FLAG_BIT_OPENED = 1,
    RS_RESOURCE_FLAG_BIT_READ_ONLY = 2,
    RS_RESOURCE_FLAG_BIT_ERROR_ON_LAST_READ = 4,
    RS_RESOURCE_FLAG_BIT_ERROR_ON_LAST_WRITE = 8,
    RS_RESOURCE_FLAG_BIT_ALLOCATED = 16,
    RS_RESOURCE_FLAG_BIT_EOR_REACHED = 32
} rs_resource_flag_bits_t;

// Rs fag bit values
typedef enum {
    RS_FLAG_BIT_DRIVER_MOUNTED = 1,
    RS_FLAG_BIT_READ_ONLY = 2
} rs_flag_bits_t;

// Operation result
typedef enum {
    RS_OP_RESULT_SUCCESS = 0,
    RS_OP_RESULT_ERROR_RESOURCE_OPENED = 1,
    RS_OP_RESULT_ERROR_RESOURCE_CLOSED = 2,
    RS_OP_RESULT_ERROR_RESOURCE_READ_ONLY = 3,
    RS_OP_RESULT_ERROR_NO_SPACE_AVAILABLE = 4,
    RS_OP_RESULT_ERROR_DRIVER_BUSY = 5,
    RS_OP_RESULT_ERROR_SEEK_OUT_OF_BOUND = 6,
    RS_OP_RESULT_ERROR_RESOURCE_DOES_NOT_ALLOCATED = 7,
    RS_OP_RESULT_ERROR_DRIVER_NOT_MOUNTED = 8
} rs_op_result_t;

// Seek position reference
typedef enum {
    RS_SEEK_ORIGIN_BEGIN = 0,
    RS_SEEK_ORIGIN_CURRENT = 1
} rs_seek_origin_t;

typedef struct {
    int_t flags;
} rs_stat_t;

// Resource system
typedef struct {
    rs_driver_t driver;
    long_int_t memory_size;
    long_int_t resource_descriptor_table_address;
    long_int_t cluster_table_address;
    long_int_t sizeof_resource_descriptor_table;
    long_int_t sizeof_cluster_table;
    int_t sizeof_resource_descriptor;
    int_t sizeof_cluster;
    int_t resource_descriptor_count;
    int_t cluster_count;
    int_t sizeof_cluster_data;
    int_t sizeof_cluster_control;
    int_t free_clusters;
    byte_t flags;
} rs_t;

// Resource
typedef struct {
    rs_resource_descriptor_t resource_descriptor;
    rs_cluster_t first_cluster;
    rs_cluster_t current_cluster;
    int_t cluster_offset;
    long_int_t size;
    long_int_t current_position;
    byte_t flags;
} rs_resource_t;

typedef struct {
    byte_t driver_mouted;
} rs_global_flags_t;

rs_global_flags_t rs_global_flags;

// Format a device
rs_op_result_t rs_format(rs_t *rs);

// Register a work area
rs_op_result_t rs_mount(rs_driver_t driver, rs_t *rs, byte_t options);

// Unregister a work area
rs_op_result_t rs_umount(rs_t *rs);

// Open/Create a resource (you must give a empty resource)
rs_op_result_t rs_open(rs_t *rs, rs_resource_code_t resource_code, rs_resource_t *resource, byte_t options);

// Close a resource
rs_op_result_t rs_close(rs_t *rs, rs_resource_t *resource);

// Read a byte from resource
byte_t rs_read(rs_t *rs, rs_resource_t *resource);

// Write a byte from resource
rs_op_result_t rs_write(rs_t *rs, rs_resource_t *resource, byte_t data);

// Move read/write pointer, (Expand resource size not implemented yet)
rs_op_result_t rs_seek(rs_t *rs, rs_resource_t *resource, rs_seek_origin_t origin, rs_seek_int_t offset);

// Truncate resource size
rs_op_result_t rs_truncate(rs_t *rs, rs_resource_t *resource);

// Flush cached data
void rs_sync(rs_t *rs, rs_resource_t *resource);

// Get descriptor status
void rs_stat(rs_t *rs, rs_resource_t *resource, rs_stat_t *stat);

// Rewind the position of a resource pointer
rs_op_result_t rs_rewind(rs_t *rs, rs_resource_t *resource);

// Create/Alloc a new resource if available
rs_resource_descriptor_t rs_alloc(rs_t *rs);

// Make a resource free to be allocated for another one
bool_t rs_release(rs_t *rs, rs_resource_t *resource);

// Get size of a resource
rs_resource_size_t rs_size(rs_resource_t *resource) ;

// Get the current read/write pointer
rs_resource_size_t rs_tell(rs_resource_t *resource);

// Test for end-of-resource on a resource
bool_t rs_eor(rs_resource_t *resource);

// Test for an error on a resource
bool_t rs_error(rs_resource_t *resource);

// Return the current available space in the partition
rs_resource_size_t rs_available_space(rs_t *rs);

// Return the total space in the partition
rs_resource_size_t rs_total_space(rs_t *rs);

#endif // RS_H
