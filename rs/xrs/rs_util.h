#ifndef RS_H
    #error The rs header file must be included.
#endif

#ifndef RS_UTIL_H
#define RS_UTIL_H

#define RS_ENVIROMENT                         1
#define RS_LOG                                1

// Read/Write a resource system table to disc
void _rs_write_rs_to_disc(rs_driver_t driver, rs_t *rs);
void _rs_read_rs_from_disc(rs_driver_t driver, rs_t *rs);

// Alloc a free cluster from disc if any
rs_memory_address_t _rs_alloc_cluster(rs_t *rs);

// Test if the given cluster is free
bool_t _rs_is_free_cluster(rs_t *rs, rs_cluster_t cluster);

// Fromat a given cluster
void _rs_format_cluster(rs_t *rs, rs_cluster_t cluster);

// Free a given cluster
void _rs_free_cluster(rs_t *rs, rs_cluster_t cluster);

// Create a chain between two clusters
void _rs_create_cluster_chain(rs_t *rs, rs_cluster_t prev_cluster, rs_cluster_t next_cluster);

// Convert resource code to rd and vice-versa
rs_resource_descriptor_t _rs_resource_code_to_resource_descriptor(rs_resource_code_t resource_code);
rs_resource_code_t _rs_resource_descriptor_to_resource_code(rs_resource_descriptor_t resource_descriptor);

// Convert cluster to address and vice-versa
rs_memory_address_t _rs_cluster_to_address(rs_t *rs, rs_cluster_t cluster);
rs_cluster_t _rs_address_to_cluster(rs_t *rs, rs_memory_address_t address);

// Convert rd to address and vice-versa
rs_memory_address_t _rs_resource_descriptor_to_address(rs_t *rs, rs_resource_descriptor_t resource_descriptor);
rs_resource_descriptor_t _rs_address_to_resource_descriptor(rs_t *rs, rs_memory_address_t address);

// Check if the end-of-resource is reached and set or clear the respecitve flag
void _rs_check_for_eor_reached(rs_resource_t *resource);

// Test the end-of-resource flag
bool_t _rs_is_eor_reached(rs_resource_t *resource);

// Check if we are at the end of resource, if yes alloc another cluster and manage the new pointers
bool_t _rs_check_for_availability(rs_t *rs, rs_resource_t *resource);

// Move the current position ahead/back 'offset' bytes
bool_t _rs_move_current_position_ahead(rs_t *rs, rs_resource_t *resource, rs_seek_int_t offset);
bool_t _rs_move_current_position_back(rs_t *rs, rs_resource_t *resource, rs_seek_int_t offset);

// Free a resource description
void _rs_format_resorce_descriptor(rs_t *rs, rs_resource_descriptor_t resource_descriptor);

// Test if given driver is mouted
bool_t _rs_is_driver_monted(rs_driver_t driver);

// Set/clear given driver as mouted
void _rs_set_driver_monted(rs_driver_t driver, bool_t is);

// Close all resources
void _rs_free_resource_descriptors(rs_t *rs);

// Close a single resources
void _rs_free_resource_descriptor(rs_t *rs, rs_resource_descriptor_t resource_descriptor);

// Decrease free cluster
void _rs_decrease_free_clusters(rs_t *rs, int_t n);

// Increase free cluster
void _rs_increase_free_clusters(rs_t *rs, int_t n);

// Free resource clustes
void _rs_format_resource_clusters(rs_t *rs, rs_resource_t resource);

// Fromat a chain of clusters
int_t _rs_format_clusters_chain(rs_t *rs, rs_cluster_t cluster);

// Get the previous cluster by a cluster
rs_cluster_t _rs_prev_cluster_by_cluster(rs_t *rs, rs_cluster_t cluster);

// Get the next cluster by a cluster
rs_cluster_t _rs_next_cluster_by_cluster(rs_t *rs, rs_cluster_t cluster);

// Get the previous cluster by a cluster address
rs_cluster_t _rs_prev_cluster_by_cluster_address(rs_t *rs, rs_memory_address_t address);

// Get the next cluster by a cluster address
rs_cluster_t _rs_next_cluster_by_cluster_address(rs_t *rs, rs_memory_address_t address);

#endif // RS_UTIL_H
