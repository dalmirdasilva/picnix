#ifndef RS_UTIL_H
    #error The header file must be included.
#endif

#ifndef RS_UTIL_C
#define RS_UTIL_C

void _rs_write_rs_to_disc(rs_driver_t driver, rs_t *rs) {
    int_t i;
    rs_memory_address_t address = 0;
    for(i = 0; i < RS_SIZEOF_RS; i++) {
        _rs_io_write(driver, address + i, *((byte_t *)(rs) + i));
    }
}

void _rs_read_rs_from_disc(rs_driver_t driver, rs_t *rs) {
    int_t i;
    rs_memory_address_t address = 0;
    for(i = 0; i < RS_SIZEOF_RS; i++) {
        *((byte_t *)(rs) + i) = _rs_io_read(driver, address + i);
    }
}

rs_memory_address_t _rs_alloc_cluster(rs_t *rs) {
    rs_memory_address_t address;
    int_t i;
    address = rs->cluster_table_address;
    for(i = 0; i < rs->cluster_count; i++) {
        if(_rs_is_free_cluster(rs, (rs_cluster_t) i)) {
            _rs_decrease_free_clusters(rs, 1);
            return address;
        }
        address += rs->sizeof_cluster;
    }
    return RS_NULL_CLUSTER_ADDRESS;
}

bool_t _rs_is_free_cluster(rs_t *rs, rs_cluster_t cluster) {
    return (cluster == _rs_prev_cluster_by_cluster(rs, cluster)) \
        && (cluster == _rs_next_cluster_by_cluster(rs, cluster));
}

void _rs_format_cluster(rs_t *rs, rs_cluster_t cluster) {
    rs_memory_address_t address;
    address = _rs_cluster_to_address(rs, cluster);
    _rs_io_write(rs->driver, CLUSTER_ADDRESS_TO_NEXT(address), cluster);
    _rs_io_write(rs->driver, CLUSTER_ADDRESS_TO_PREV(address), cluster);
}

void _rs_free_cluster(rs_t *rs, rs_cluster_t cluster) {
    _rs_format_cluster(rs, cluster);
    _rs_increase_free_clusters(rs, 1);
}

void _rs_create_cluster_chain(rs_t *rs, rs_cluster_t prev_cluster, rs_cluster_t next_cluster) {
    rs_memory_address_t address;
    if(prev_cluster != RS_INEXISTENT_CLUSTER) {
        address = _rs_cluster_to_address(rs, prev_cluster);
        _rs_io_write(rs->driver, CLUSTER_ADDRESS_TO_NEXT(address), (byte_t) next_cluster);
    }
    if(next_cluster != RS_INEXISTENT_CLUSTER) {
        address = _rs_cluster_to_address(rs, next_cluster);
        _rs_io_write(rs->driver, CLUSTER_ADDRESS_TO_PREV(address), (byte_t) prev_cluster);
    }
}

rs_resource_descriptor_t _rs_resource_code_to_resource_descriptor(rs_resource_code_t resource_code) {
    return (rs_resource_descriptor_t) resource_code;
}

rs_resource_code_t _rs_resource_descriptor_to_resource_code(rs_resource_descriptor_t resource_descriptor) {
    return (rs_resource_code_t) resource_descriptor;
}

rs_memory_address_t _rs_cluster_to_address(rs_t *rs, rs_cluster_t cluster) {
    return (cluster * rs->sizeof_cluster) + rs->cluster_table_address;
}

rs_cluster_t _rs_address_to_cluster(rs_t *rs, rs_memory_address_t address) {
    return (address - rs->cluster_table_address) / rs->sizeof_cluster;   
}

rs_memory_address_t _rs_resource_descriptor_to_address(rs_t *rs, rs_resource_descriptor_t resource_descriptor) {
    return (resource_descriptor * rs->sizeof_resource_descriptor) + rs->resource_descriptor_table_address;
}

rs_resource_descriptor_t _rs_address_to_resource_descriptor(rs_t *rs, rs_memory_address_t address) {
    return (address - rs->resource_descriptor_table_address) / rs->sizeof_resource_descriptor;
}

void _rs_check_for_eor_reached(rs_resource_t *resource) {
    if(resource->current_position >= resource->size) {
        resource->flags |= RS_RESOURCE_FLAG_BIT_EOR_REACHED;
    } else {
        resource->flags &= ~RS_RESOURCE_FLAG_BIT_EOR_REACHED;
    }
}

bool_t _rs_is_eor_reached(rs_resource_t *resource) {
    return resource->flags & RS_RESOURCE_FLAG_BIT_EOR_REACHED;
}

bool_t _rs_check_for_availability(rs_t *rs, rs_resource_t *resource) {
    rs_memory_address_t address;
    rs_cluster_t cluster;
    _rs_check_for_eor_reached(resource);
    if(resource->cluster_offset >= rs->sizeof_cluster) {
        if(rs_eor(resource)) {
            address = _rs_alloc_cluster(rs);
            if(address == RS_NULL_CLUSTER_ADDRESS) {
                return 0;
            }
            cluster = _rs_address_to_cluster(rs, address);
            _rs_create_cluster_chain(rs, resource->current_cluster, cluster);
            resource->current_cluster = cluster;
        } else {
            resource->current_cluster = _rs_next_cluster_by_cluster(rs, resource->current_cluster);
        }
        resource->cluster_offset = rs->sizeof_cluster_control;
    }
    return 1;
}

bool_t _rs_move_current_position_ahead(rs_t *rs, rs_resource_t *resource, rs_seek_int_t offset) {
    int_t until_the_end;
    int_t how_many_clustes_ahead;
    int_t i;
    resource->current_position += offset;
    until_the_end = (rs->sizeof_cluster - resource->cluster_offset);
    if(offset <= until_the_end) {
        resource->cluster_offset += offset;
        return 1;
    }
    offset -= until_the_end;
    how_many_clustes_ahead = (offset / rs->sizeof_cluster_data);
    if((offset % rs->sizeof_cluster_data) != 0) {
        how_many_clustes_ahead++;
    }
    resource->cluster_offset = (offset % rs->sizeof_cluster_data) + rs->sizeof_cluster_control;
    for(i = 0; i < how_many_clustes_ahead; i++) {
        resource->current_cluster = _rs_next_cluster_by_cluster(rs, resource->current_cluster);
    }
    return 1;
}

bool_t _rs_move_current_position_back(rs_t *rs, rs_resource_t *resource, rs_seek_int_t offset) {
    int_t until_the_begin;
    int_t how_many_clustes_back;
    int_t i;
    resource->current_position -= offset;
    until_the_begin = (resource->cluster_offset - rs->sizeof_cluster_control);
    if(offset <= until_the_begin) {
        resource->cluster_offset -= offset;
        return 1;
    }
    offset -= until_the_begin;
    how_many_clustes_back = (offset / rs->sizeof_cluster_data);
    if((offset % rs->sizeof_cluster_data) != 0) {
        how_many_clustes_back++;
    }
    resource->cluster_offset = rs->sizeof_cluster - (offset % rs->sizeof_cluster_data) - 1;
    for(i = 0; i < how_many_clustes_back; i++) {
        resource->current_cluster = _rs_prev_cluster_by_cluster(rs, resource->current_cluster);
    }
    return 1;
}

void _rs_format_resorce_descriptor(rs_t *rs, rs_resource_descriptor_t resource_descriptor) {
    int i;
    rs_memory_address_t address;
    address = _rs_resource_descriptor_to_address(rs, resource_descriptor);
    for(i = 0; i < rs->sizeof_resource_descriptor; i++) {
        _rs_io_write(rs->driver, address + i, 0x00);
    }
}

bool_t _rs_is_driver_monted(rs_driver_t driver) {
    return rs_global_flags.driver_mouted & (1 << driver);
}

void _rs_set_driver_monted(rs_driver_t driver, bool_t is) {
    if(is) {
        rs_global_flags.driver_mouted |= (1 << driver);
    } else {
        rs_global_flags.driver_mouted &= ~(1 << driver);
    }
}

void _rs_free_resource_descriptors(rs_t *rs) {
    int_t i;
    for(i = 0; i < rs->resource_descriptor_count; i++) {
        _rs_free_resource_descriptor(rs, i);
    }
}

void _rs_free_resource_descriptor(rs_t *rs, rs_resource_descriptor_t resource_descriptor) {
    rs_memory_address_t address;
    byte_t flags;
    address = _rs_resource_descriptor_to_address(rs, resource_descriptor);
    flags = _rs_io_read(rs->driver, RD_ADDRESS_TO_FLAG(address));
    flags &= ~(RS_RESOURCE_FLAG_BIT_OPENED | RS_RESOURCE_FLAG_BIT_READ_ONLY);
    _rs_io_write(rs->driver, RD_ADDRESS_TO_FLAG(address), flags);
}

void _rs_decrease_free_clusters(rs_t *rs, int_t n) {
    rs->free_clusters -= n;
    _rs_write_rs_to_disc(rs->driver, rs);
}

void _rs_increase_free_clusters(rs_t *rs, int_t n) {
    rs->free_clusters += n;
    _rs_write_rs_to_disc(rs->driver, rs);
}

void _rs_format_resource_clusters(rs_t *rs, rs_resource_t *resource) {
    int_t freed_clusters;
    freed_clusters = _rs_format_clusters_chain(rs, resource->first_cluster);
    _rs_increase_free_clusters(rs, freed_clusters);
}

int_t _rs_format_clusters_chain(rs_t *rs, rs_cluster_t cluster) {
    rs_cluster_t next_cluster;
    int_t formated_clusters = 0;
    do {
        next_cluster = _rs_next_cluster_by_cluster(rs, cluster);
        _rs_format_cluster(rs, cluster);
        formated_clusters++;
        if(next_cluster == RS_INEXISTENT_CLUSTER || next_cluster == cluster) {
            break;
        }
        cluster = next_cluster;
    } while(1);
    return formated_clusters;
}

rs_cluster_t _rs_prev_cluster_by_cluster(rs_t *rs, rs_cluster_t cluster) {
    return _rs_prev_cluster_by_cluster_address(rs, _rs_cluster_to_address(rs, cluster));
}

rs_cluster_t _rs_next_cluster_by_cluster(rs_t *rs, rs_cluster_t cluster) {
    return _rs_next_cluster_by_cluster_address(rs, _rs_cluster_to_address(rs, cluster));
}

rs_cluster_t _rs_prev_cluster_by_cluster_address(rs_t *rs, rs_memory_address_t address) {
    return _rs_io_read(rs->driver, CLUSTER_ADDRESS_TO_PREV(address));
}

rs_cluster_t _rs_next_cluster_by_cluster_address(rs_t *rs, rs_memory_address_t address) {
    return _rs_io_read(rs->driver, CLUSTER_ADDRESS_TO_NEXT(address));
}

#endif // RS_UTIL_C
