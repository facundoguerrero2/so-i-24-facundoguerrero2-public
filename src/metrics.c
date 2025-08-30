#include "metrics.h"

#define ONE_SEC 1000 // one second in ms

double get_memory_usage()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long total_mem = 0, free_mem = 0;

    // Abrir el archivo /proc/meminfo
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/meminfo");
        return -1.0;
    }

    // Leer los valores de memoria total y disponible
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "MemTotal: %llu kB", &total_mem) == 1)
        {
            continue; // MemTotal encontrado
        }
        if (sscanf(buffer, "MemAvailable: %llu kB", &free_mem) == 1)
        {
            break; // MemAvailable encontrado, podemos dejar de leer
        }
    }

    fclose(fp);

    // Verificar si se encontraron ambos valores
    if (total_mem == 0 || free_mem == 0)
    {
        fprintf(stderr, "Error al leer la información de memoria desde /proc/meminfo\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de memoria
    double used_mem = total_mem - free_mem;
    double mem_usage_percent = (used_mem / total_mem) * 100.0;

    return mem_usage_percent;
}

double get_cpu_usage()
{
    static unsigned long long prev_user = 0, prev_nice = 0, prev_system = 0, prev_idle = 0, prev_iowait = 0,
                              prev_irq = 0, prev_softirq = 0, prev_steal = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long totald, idled;
    double cpu_usage_percent;

    // Abrir el archivo /proc/stat
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    char buffer[BUFFER_SIZE * 4];
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        perror("Error al leer /proc/stat");
        fclose(fp);
        return -1.0;
    }
    fclose(fp);

    // Analizar los valores de tiempo de CPU
    int ret = sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait,
                     &irq, &softirq, &steal);
    if (ret < 8)
    {
        fprintf(stderr, "Error al parsear /proc/stat\n");
        return -1.0;
    }

    // Calcular las diferencias entre las lecturas actuales y anteriores
    unsigned long long prev_idle_total = prev_idle + prev_iowait;
    unsigned long long idle_total = idle + iowait;

    unsigned long long prev_non_idle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq + prev_steal;
    unsigned long long non_idle = user + nice + system + irq + softirq + steal;

    unsigned long long prev_total = prev_idle_total + prev_non_idle;
    unsigned long long total = idle_total + non_idle;

    totald = total - prev_total;
    idled = idle_total - prev_idle_total;

    if (totald == 0)
    {
        fprintf(stderr, "Totald es cero, no se puede calcular el uso de CPU!\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de CPU
    cpu_usage_percent = ((double)(totald - idled) / totald) * 100.0;

    // Actualizar los valores anteriores para la siguiente lectura
    prev_user = user;
    prev_nice = nice;
    prev_system = system;
    prev_idle = idle;
    prev_iowait = iowait;
    prev_irq = irq;
    prev_softirq = softirq;
    prev_steal = steal;

    return cpu_usage_percent;
}

unsigned long long get_process()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long processes = 0;

    // Abrir el archivo /proc/stat
    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1;
    }

    // Leer los valores de processos
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "processes %llu", &processes) == 1)
        {
            break; // processes encontrado, podemos dejar de leer
        }
    }

    fclose(fp);
    // Verificar si se encontro el valor
    if (processes < 0)
    {
        fprintf(stderr, "Error al leer la información de disco desde /proc/stat\n");
        return -1;
    }

    return processes;
}

double get_avg_read_disk_time()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long time_spent_reading = 0, reads_completed = 0;
    char disk_name[64];

    // Abrir el archivo /proc/diskstats
    fp = fopen("/proc/diskstats", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/diskstats");
        return -1.0;
    }

    // Leer los valores tiempo escrito de disco
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "%*d %*d %s %llu %*u %*u %llu %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u",
                   disk_name, &reads_completed, &time_spent_reading) == 3 &&
            strstr(disk_name, "loop") == NULL)
        {

            break; // disco encontrado, podemos dejar de leer
        }
    }

    fclose(fp);

    // Verificar si se encontraron ambos valores
    if (time_spent_reading < 0 || reads_completed <= 0)
    {
        fprintf(stderr, "Error al leer la información de disco desde /proc/diskstats\n");
        return -1.0;
    }

    // Calcular el tiempo promedio de lectura
    double avg_read_time = (double)time_spent_reading / reads_completed;

    return avg_read_time;
}

double get_avg_write_disk_time()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long time_spent_writing = 0, writes_completed = 0;
    char disk_name[64];

    // Abrir el archivo /proc/diskstats
    fp = fopen("/proc/diskstats", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/diskstats");
        return -1.0;
    }

    // Leer los valores tiempo escrito de disco
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "%*d %*d %s %*u %*u %*u %*u %llu %*u %*u %llu %*u %*u %*u %*u %*u %*u %*u %*u %*u",
                   disk_name, &writes_completed, &time_spent_writing) == 2 &&
            strstr(disk_name, "loop") == NULL)
        {
            break; // disco encontrado, podemos dejar de leer
        }
    }

    fclose(fp);
    // Verificar si se encontraron ambos valores
    if (time_spent_writing <= 0 || writes_completed <= 0)
    {
        fprintf(stderr, "Error al leer la información de disco desde /proc/diskstats\n");
        return -1.0;
    }

    // Calcular el tiempo promedio de lectura
    double avg_write_time = (double)time_spent_writing / writes_completed;

    return avg_write_time;
}

double get_disk_IO_percentage_time()
{
    static unsigned long busy_previous_time = 0;
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long time_IO = 0;
    char disk_name[64];

    // Abrir el archivo /proc/diskstats
    fp = fopen("/proc/diskstats", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/diskstats");
        return -1.0;
    }

    // Leer los valores tiempo ponderado procesando IOs
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "%*d %*d %s %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %llu %*u %*u %*u %*u %*u %*u", disk_name,
                   &time_IO) == 1 &&
            strstr(disk_name, "loop") == NULL)
        {
            break; // disco encontrado, podemos dejar de leer
        }
    }

    fclose(fp);
    // Verificar si se encontro el valor
    if (time_IO <= 0)
    {
        fprintf(stderr, "Error al leer la información de disco desde /proc/diskstats\n");
        return -1.0;
    }

    // Calcular el porcentaje de tiempo que esta procesando IOs
    double percent_IO_time = ((double)(time_IO - busy_previous_time) / ONE_SEC) * 100;
    busy_previous_time = time_IO;

    return percent_IO_time;
}

double get_net_receive_kbps()
{
    static double old_receive_byte_value = 0;
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long received_bytes = 0;

    // Abrir el archivo /proc/net/dev
    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/net/dev");
        return -1.0;
    }

    // Leer los valores tiempo ponderado procesando IOs
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "enp%*s %llu %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u",
                   &received_bytes) == 1)
        {
            break;
        }
    }

    fclose(fp);
    // Verificar si se encontro el valor
    if (received_bytes < 0)
    {
        fprintf(stderr, "Error al leer la información de disco desde /proc/net/dev\n");
        return -1.0;
    }

    // Calcular el resultado em kb/s
    double received_kbps = (received_bytes - old_receive_byte_value) / ONE_SEC;
    old_receive_byte_value = received_bytes;

    return received_kbps;
}

double get_net_sent_kbps()
{
    static double old_sent_byte_value = 0;
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long sent_bytes = 0;

    // Abrir el archivo /proc/net/dev"
    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/net/dev");
        return -1.0;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "enp%*s %*u %*u %*u %*u %*u %*u %*u %*u %llu %*u %*u %*u %*u %*u %*u %*u", &sent_bytes) == 1)
        {
            break;
        }
    }

    fclose(fp);
    // Verificar si se encontro el valor
    if (sent_bytes < 0)
    {
        fprintf(stderr, "Error al leer la información de disco desde /proc/net/dev\n");
        return -1.0;
    }

    // Calcular el resultado em kb/s
    double sent_kbps = (sent_bytes - old_sent_byte_value) / ONE_SEC;
    old_sent_byte_value = sent_bytes;

    return sent_kbps;
}

unsigned long long get_net_received_packets()
{
    static unsigned long long previous_received_packets = 0;
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long received_packets = 0;

    // Abrir el archivo /proc/net/dev
    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/net/dev");
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "enp%*s %*u %llu %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u",
                   &received_packets) == 1)
        {
            break;
        }
    }

    fclose(fp);
    // Verificar si se encontro el valor
    if (received_packets < 0)
    {
        fprintf(stderr, "Error al leer la información de disco desde /proc/net/dev\n");
        return -1;
    }

    unsigned long long total_received_packages = received_packets - previous_received_packets;
    previous_received_packets = received_packets;
    return total_received_packages;
}

unsigned long long get_net_sent_packets()
{
    static unsigned long long previous_sent_packets = 0;
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long sent_packets = 0;

    // Abrir el archivo /proc/net/dev"
    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/net/dev");
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "enp%*s %*u %*u %*u %*u %*u %*u %*u %*u %*u %llu %*u %*u %*u %*u %*u %*u", &sent_packets) ==
            1)
        {
            break;
        }
    }

    fclose(fp);
    // Verificar si se encontro el valor
    if (sent_packets < 0)
    {
        fprintf(stderr, "Error al leer la información de disco desde /proc/net/dev\n");
        return -1;
    }

    unsigned long long total_sent_packages = sent_packets - previous_sent_packets;
    previous_sent_packets = sent_packets;
    return total_sent_packages;
}


double get_fragmentation()
{   
    int memory_val_fifo;
    int memory_values[MEMORY_REPORTED_VALUES_SIZE] = {0,0,0,0};

    memory_val_fifo = open(FIFO_MEMORY_VALUES, O_RDONLY);

    read(memory_val_fifo, memory_values, sizeof(memory_values));

    close(memory_val_fifo);
    return (double)memory_values[0];
}

unsigned long long get_first_fit_counter()
{
    int memory_val_fifo, memory_val_fifo_ack;
    int memory_values[MEMORY_REPORTED_VALUES_SIZE] = {0,0,0,0};

    memory_val_fifo = open(FIFO_MEMORY_VALUES, O_RDONLY);
    memory_val_fifo_ack = open(FIFO_MEMORY_VALUES_ACK, O_WRONLY);

    read(memory_val_fifo, memory_values, sizeof(memory_values));
    write(memory_val_fifo_ack, memory_values, sizeof(memory_values));

    close(memory_val_fifo);
    close(memory_val_fifo_ack);
    return (unsigned long long)memory_values[1];
}

unsigned long long get_best_fit_counter()
{
    int memory_val_fifo, memory_val_fifo_ack;
    int memory_values[MEMORY_REPORTED_VALUES_SIZE] = {0,0,0,0};

    memory_val_fifo = open(FIFO_MEMORY_VALUES, O_RDONLY);
    memory_val_fifo_ack = open(FIFO_MEMORY_VALUES_ACK, O_WRONLY);

    read(memory_val_fifo, memory_values, sizeof(memory_values));
    write(memory_val_fifo_ack, memory_values, sizeof(memory_values));

    close(memory_val_fifo);
    close(memory_val_fifo_ack);
    return (unsigned long long)memory_values[2];
}

unsigned long long get_worst_fit_counter()
{
    int memory_val_fifo, memory_val_fifo_ack;
    int memory_values[MEMORY_REPORTED_VALUES_SIZE] = {0,0,0,0};

    memory_val_fifo = open(FIFO_MEMORY_VALUES, O_RDONLY);
    memory_val_fifo_ack = open(FIFO_MEMORY_VALUES_ACK, O_WRONLY);

    read(memory_val_fifo, memory_values, sizeof(memory_values));
    write(memory_val_fifo_ack, memory_values, sizeof(memory_values));

    close(memory_val_fifo);
    close(memory_val_fifo_ack);
    return (unsigned long long)memory_values[3];
}
