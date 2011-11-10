#ifndef RS_H
    #error The rs header file must be included.
#endif

#ifndef RS_IO_C
#define RS_IO_C

#if VIRTUAL_ENVIROMENT == 1
 #define VIRTUAL_DRIVER_COUNT 4
 #define VIRTUAL_DRIVER_MEMORY 8192
#else
 #ifndef LIBRARY_MULTI_EXEEPROM_H
   #error LIBRARY_MULTI_EXEEPROM_H must be included.
 #endif
#endif

#if VIRTUAL_ENVIROMENT == 1
FILE *fp;
char file[] = "img";
#endif

byte_t _rs_io_read(rs_driver_t driver, rs_memory_address_t address) {

#if VIRTUAL_ENVIROMENT == 1
    byte_t data;
    if((fp = fopen(file, "rb")) == NULL) {
        printf("Error in read.\n");
        exit(1);
    }
    fseek(fp, address, 0);
    fread(&data, sizeof(data), 1, fp);
    fclose(fp);
    return data;
#else
    switch(driver) {
        case RS_DRIVER_MULTI_EXTERNAL_EEPROM:
            return multi_exeeprom_read(address);
        break;
        case RS_DRIVER_SELF_EEPROM:
            return ieeprom_read(address);
        break;
    }
    return 0;
#endif
}

void _rs_io_write(rs_driver_t driver, rs_memory_address_t address, byte_t data) {
    
#if VIRTUAL_ENVIROMENT == 1
    if((fp = fopen(file, "r+b")) == NULL) {
        printf("Error in write.\n");
        exit(1);
    }
    fseek(fp, address, 0);
    fwrite(&data, sizeof(data), 1, fp);
    fclose(fp);
#else
    switch(driver) {
        case RS_DRIVER_MULTI_EXTERNAL_EEPROM:
            multi_exeeprom_write(address, data);
        break;
        case RS_DRIVER_SELF_EEPROM:
            ieeprom_write(address, data);
        break;
    }
#endif
}

#if VIRTUAL_ENVIROMENT == 1

char* itob(int i) {
    int bits;
    int j, k;
    long_int_t mi = 0;
    mi |= i;
    static char buff[sizeof(mi) * 8 + 1];
    bits = sizeof(mi) * 8;
    for(j = bits - 1, k = 0; j >= 0; j--, k++) {
        buff[k] = ((mi >> j) & 0x01) + '0';
    }
    buff[bits] = '\0';
    return buff;
}

void _rs_io_memory_dump(rs_t *rs) {
    rs_memory_address_t memory_address;
    long_int_t count, count2;
    FILE *fp;
    if(!_rs_is_driver_monted(rs->driver)) {
        printf("Rs not mouted yet\n");
        return;
    }
    fp = fopen("dump", "w+");
    fprintf(fp, "DRIVER: %x\n", rs->driver);
    fprintf(fp, "\n==========================\n");
    fprintf(fp, "\nRs\n");
    fprintf(fp, "-----------------\n");
    fprintf(fp, "memory_size:                       0x%04x %4d %s\n", rs->memory_size, rs->memory_size, itob(rs->memory_size));
    fprintf(fp, "resource_descriptor_table_address: 0x%04x %4d %s\n", rs->resource_descriptor_table_address, rs->resource_descriptor_table_address, itob(rs->resource_descriptor_table_address));
    fprintf(fp, "cluster_table_address:             0x%04x %4d %s\n", rs->cluster_table_address, rs->cluster_table_address, itob(rs->cluster_table_address));
    fprintf(fp, "sizeof_resource_descriptor_table:  0x%04x %4d %s\n", rs->sizeof_resource_descriptor_table, rs->sizeof_resource_descriptor_table, itob(rs->sizeof_resource_descriptor_table));
    fprintf(fp, "sizeof_cluster_table:              0x%04x %4d %s\n", rs->sizeof_cluster_table, rs->sizeof_cluster_table, itob(rs->sizeof_resource_descriptor_table));
    fprintf(fp, "sizeof_resource_descriptor:        0x%04x %4d %s\n", rs->sizeof_resource_descriptor, rs->sizeof_resource_descriptor, itob(rs->sizeof_resource_descriptor));
    fprintf(fp, "sizeof_cluster:                    0x%04x %4d %s\n", rs->sizeof_cluster, rs->sizeof_cluster, itob(rs->sizeof_cluster));
    fprintf(fp, "resource_descriptor_count:         0x%04x %4d %s\n", rs->resource_descriptor_count, rs->resource_descriptor_count, itob(rs->resource_descriptor_count));
    fprintf(fp, "cluster_count:                     0x%04x %4d %s\n", rs->cluster_count, rs->cluster_count, itob(rs->cluster_count));
    fprintf(fp, "sizeof_cluster_data:               0x%04x %4d %s\n", rs->sizeof_cluster_data, rs->sizeof_cluster_data, itob(rs->sizeof_cluster_data));
    fprintf(fp, "sizeof_cluster_control:            0x%04x %4d %s\n", rs->sizeof_cluster_control, rs->sizeof_cluster_control, itob(rs->sizeof_cluster_control));
    fprintf(fp, "free_clusters:                     0x%04x %4d %s\n", rs->free_clusters, rs->free_clusters, itob(rs->free_clusters));
    fprintf(fp, "flags:                             0x%04x %4d %s\n", rs->flags, rs->flags, itob(rs->flags));
    fprintf(fp, "\n==========================\n");
    fprintf(fp, "\nResource table\n");
    fprintf(fp, "-----------------\n");
    count = 0;
    for(
        memory_address = rs->resource_descriptor_table_address;
        memory_address < (rs->resource_descriptor_table_address + rs->sizeof_resource_descriptor_table);
        memory_address++
   ) {
          
        if((count % rs->sizeof_resource_descriptor) == 0) {
            fprintf(fp, "\n%02x: ", (count) ? count / rs->sizeof_resource_descriptor : 0);
        }
        fprintf(fp, "%02x ", _rs_io_read(rs->driver, memory_address));
        count++;
    }
    fprintf(fp, "\n==========================\n");
    fprintf(fp, "\nCluster table\n");
    fprintf(fp, "-----------------\n");
    fprintf(fp, "\n    |nn |pp |dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd");
    fprintf(fp, "\n    --------------------------------------------------------------------------------------------------");
    count = 0;
    for(
        memory_address = rs->cluster_table_address;
        memory_address < (rs->cluster_table_address + rs->sizeof_cluster_table);
        memory_address++
   ) {
        if((count % rs->sizeof_cluster) == 0) {
            fprintf(fp, "\n%02x: |", (count) ? count / rs->sizeof_cluster : 0);
            count2 = 0;
        }
        if(count2 == 1 || count2 == 2) {
            fprintf(fp, "|");
        }
        fprintf(fp, "%02x ", _rs_io_read(rs->driver, memory_address));
        count++;
        count2++;
    }
    fclose(fp);
}
#endif // VIRTUAL_ENVIROMENT

#endif // RS_IO_C
