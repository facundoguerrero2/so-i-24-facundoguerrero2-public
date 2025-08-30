#include "expose_metrics.h"

/** Mutex para sincronización de hilos */
pthread_mutex_t lock;

/** Métrica de Prometheus para el uso de CPU */
static prom_gauge_t* cpu_usage_metric;

/** Métrica de Prometheus para el uso de memoria */
static prom_gauge_t* memory_usage_metric;

/** Métrica de Prometheus para el tiempo promedio de lectura del disco */
static prom_gauge_t* avg_read_time_metric;

/** Métrica de Prometheus para el tiempo promedio de escritura del disco */
static prom_gauge_t* avg_write_time_metric;

/** Métrica de Prometheus para el porcentaje de uso del disco */
static prom_gauge_t* disk_IO_percentage_time_metric;

/** Métrica de Prometheus para la velocidad en kbps de recepcion de bytes de internet */
static prom_gauge_t* net_receive_kbps_metric;

/** Métrica de Prometheus para la velocidad en kbps de envio de bytes de internet */
static prom_gauge_t* net_sent_kbps_metric;

/** Métrica de Prometheus para la cantidad de packetes recibidos de internet */
static prom_counter_t* net_received_packets_metric;

/** Métrica de Prometheus para la cantidad de packetes enviados de internet */
static prom_counter_t* net_sent_packets_metric;

/** Métrica de Prometheus para la cantidad de procesos en ejecucion */
static prom_gauge_t* processes_metric;

/** Métrica de Prometheus para el porcentaje de fragmentacion de memoria */
static prom_gauge_t* fragmentation_metric;

/** Métrica de Prometheus para la cantidad de veces que se uso el algoritmo first fit */
static prom_counter_t* first_fit_metric;

/** Métrica de Prometheus para la cantidad de veces que se uso el algoritmo best fit */
static prom_counter_t* best_fit_metric;

/** Métrica de Prometheus para la cantidad de veces que se uso el algoritmo worst fit */
static prom_counter_t* worst_fit_metric;

double update_cpu_gauge()
{
    double usage = get_cpu_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(cpu_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de CPU\n");
    }
    return usage;
}

double update_processes_gauge()
{
    double processes = get_process();
    if (processes > 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(processes_metric, processes, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el porcentaje de tiempo tomado en procesos IO\n");
    }
    return processes;
}

double update_memory_gauge()
{
    double usage = get_memory_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(memory_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de memoria\n");
    }
    return usage;
}

double update_disk_avg_read_time()
{
    double avg_read_time = get_avg_read_disk_time();
    if (avg_read_time >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(avg_read_time_metric, avg_read_time, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el tiempo de lectura promedio\n");
    }
    return avg_read_time;
}

double update_disk_avg_write_time()
{
    double avg_write_time = get_avg_write_disk_time();
    if (avg_write_time >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(avg_write_time_metric, avg_write_time, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el tiempo de escritura promedio\n");
    }
    return avg_write_time;
}

double update_disk_IO_percentage_time()
{

    double disk_IO_percentage_time = get_disk_IO_percentage_time();
    if (disk_IO_percentage_time >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(disk_IO_percentage_time_metric, disk_IO_percentage_time, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el porcentaje de tiempo tomado en procesos IO\n");
    }
    return disk_IO_percentage_time;
}

double update_net_receive_kbps()
{

    double net_receive_kbps = get_net_receive_kbps();
    if (net_receive_kbps >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(net_receive_kbps_metric, net_receive_kbps, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtner kbps recibidos IO\n");
    }
    return net_receive_kbps;
}

double update_net_sent_kbps()
{

    double net_sent_kbps = get_net_sent_kbps();
    if (net_sent_kbps >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(net_sent_kbps_metric, net_sent_kbps, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtner kbps enviados IO\n");
    }
    return net_sent_kbps;
}

unsigned long long update_net_received_packets()
{
    unsigned long long net_received_packets = get_net_received_packets();
    if (net_received_packets >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_counter_add(net_received_packets_metric, net_received_packets, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtner kbps enviados IO\n");
    }
    return net_received_packets;
}

unsigned long long update_net_sent_packets()
{
    unsigned long long net_sent_packets = get_net_sent_packets();
    if (net_sent_packets >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_counter_add(net_sent_packets_metric, net_sent_packets, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtner kbps enviados IO\n");
    }
    return net_sent_packets;
}

double update_fragmentation_gauge()
{
    double fragmentation = get_fragmentation();
    if (fragmentation >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(fragmentation_metric, fragmentation, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el porcentaje de fragmentacion de memoria\n");
    }
    return fragmentation;
}

unsigned long long update_political_fit_counters()
{
    static double prev_first_fit = 0;
    static double prev_best_fit = 0;
    static double prev_worst_fit = 0;
    
    double first_fit = get_first_fit_counter();
    double best_fit = get_best_fit_counter();
    double worst_fit = get_worst_fit_counter();
    
    pthread_mutex_lock(&lock);
    
    // Solo sumar el incremento desde la última lectura
    if (first_fit > prev_first_fit) {
        prom_counter_add(first_fit_metric, first_fit - prev_first_fit, NULL);
        prev_first_fit = first_fit;
    }
    
    if (best_fit > prev_best_fit) {
        prom_counter_add(best_fit_metric, best_fit - prev_best_fit, NULL);
        prev_best_fit = best_fit;
    }
    
    if (worst_fit > prev_worst_fit) {
        prom_counter_add(worst_fit_metric, worst_fit - prev_worst_fit, NULL);
        prev_worst_fit = worst_fit;
    }
    
    pthread_mutex_unlock(&lock);
    
    return (unsigned long long)(first_fit + best_fit + worst_fit);
}

void* expose_metrics(void* arg)
{
    (void)arg; // Argumento no utilizado

    // Aseguramos que el manejador HTTP esté adjunto al registro por defecto
    promhttp_set_active_collector_registry(NULL);

    // Iniciamos el servidor HTTP en el puerto 8000
    struct MHD_Daemon* daemon = promhttp_start_daemon(MHD_USE_SELECT_INTERNALLY, 8080, NULL, NULL);
    if (daemon == NULL)
    {
        fprintf(stderr, "Error al iniciar el servidor HTTP\n");
        return NULL;
    }

    // Mantenemos el servidor en ejecución
    while (1)
    {
        sleep(1);
    }

    // Nunca debería llegar aquí
    MHD_stop_daemon(daemon);
    return NULL;
}

int init_metrics()
{
    // Inicializamos el mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        fprintf(stderr, "Error al inicializar el mutex\n");
        return EXIT_FAILURE;
    }

    // Inicializamos el registro de coleccionistas de Prometheus
    if (prom_collector_registry_default_init() != 0)
    {
        fprintf(stderr, "Error al inicializar el registro de Prometheus\n");
        return EXIT_FAILURE;
    }

    // Creamos la métrica para el uso de CPU
    cpu_usage_metric = prom_gauge_new("cpu_usage_percentage", "Porcentaje de uso de CPU", 0, NULL);
    if (cpu_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de CPU\n");
        return EXIT_FAILURE;
    }

    // Creamos la métrica para el uso de memoria
    memory_usage_metric = prom_gauge_new("memory_usage_percentage", "Porcentaje de uso de memoria", 0, NULL);
    if (memory_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de memoria\n");
        return EXIT_FAILURE;
    }

    // Creamos la métrica para el tiempo de lectura promedio
    avg_read_time_metric = prom_gauge_new("avg_read_time", "Tiempo de lectura promedio", 0, NULL);
    if (avg_read_time_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica tiempo promedio de lectura de disco\n");
        return EXIT_FAILURE;
    }

    // Creamos la métrica para el tiempo de escritura promedio
    avg_write_time_metric = prom_gauge_new("avg_write_time", "Tiempo de escritura promedio", 0, NULL);
    if (avg_write_time_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica tiempo promedio de escritura de disco\n");
        return EXIT_FAILURE;
    }

    disk_IO_percentage_time_metric =
        prom_gauge_new("disk_IO_percentage_time", "Porcentaje de tiempo tomado en IOs", 0, NULL);
    if (disk_IO_percentage_time_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica tiempo promedio de escritura de disco\n");
        return EXIT_FAILURE;
    }

    net_receive_kbps_metric = prom_gauge_new("net_receive_kbps", "kbps recibidos", 0, NULL);
    if (net_receive_kbps_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de kbps recibidos\n");
        return EXIT_FAILURE;
    }

    net_sent_kbps_metric = prom_gauge_new("net_sent_kbps_metric", "kbps enviados", 0, NULL);
    if (net_sent_kbps_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de kbps recibidos\n");
        return EXIT_FAILURE;
    }

    net_received_packets_metric = prom_counter_new("net_received_packets_metric", "Packetes recibidos", 0, NULL);
    if (net_received_packets_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de kbps recibidos\n");
        return EXIT_FAILURE;
    }

    net_sent_packets_metric = prom_counter_new("net_sent_packets_metric", "Packetes enviados", 0, NULL);
    if (net_sent_packets_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de kbps recibidos\n");
        return EXIT_FAILURE;
    }

    processes_metric = prom_gauge_new("processes_metric", "Procesos ejecutandose", 0, NULL);
    if (processes_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de kbps recibidos\n");
        return EXIT_FAILURE;
    }

    fragmentation_metric = prom_gauge_new("memory_fragmentation_percentage", "Porcentaje de fragmentacion de memoria", 0, NULL);
    if (fragmentation_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de fragmentacion de memoria\n");
        return EXIT_FAILURE;
    }

    first_fit_metric = prom_counter_new("first_fit_count", "Cantidad de veces que se uso el algoritmo first fit", 0, NULL);
    if (first_fit_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de first fit\n");
        return EXIT_FAILURE;
    }

    best_fit_metric = prom_counter_new("best_fit_count", "Cantidad de veces que se uso el algoritmo best fit", 0, NULL);
    if (best_fit_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de best fit\n");
        return EXIT_FAILURE;
    }

    worst_fit_metric = prom_counter_new("worst_fit_count", "Cantidad de veces que se uso el algoritmo worst fit", 0, NULL);
    if (worst_fit_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de worst fit\n");
        return EXIT_FAILURE;
    }


    // Registramos las métricas en el registro por defecto
    if (prom_collector_registry_must_register_metric(cpu_usage_metric) == NULL ||
        prom_collector_registry_must_register_metric(memory_usage_metric) == NULL ||
        prom_collector_registry_must_register_metric(avg_read_time_metric) == NULL ||
        prom_collector_registry_must_register_metric(avg_write_time_metric) == NULL ||
        prom_collector_registry_must_register_metric(disk_IO_percentage_time_metric) == NULL ||
        prom_collector_registry_must_register_metric(net_receive_kbps_metric) == NULL ||
        prom_collector_registry_must_register_metric(net_sent_kbps_metric) == NULL ||
        prom_collector_registry_must_register_metric(net_received_packets_metric) == NULL ||
        prom_collector_registry_must_register_metric(net_sent_packets_metric) == NULL ||
        prom_collector_registry_must_register_metric(processes_metric) == NULL ||
        prom_collector_registry_must_register_metric(fragmentation_metric) == NULL ||
        prom_collector_registry_must_register_metric(first_fit_metric) == NULL ||
        prom_collector_registry_must_register_metric(best_fit_metric) == NULL ||
        prom_collector_registry_must_register_metric(worst_fit_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void destroy_mutex()
{
    pthread_mutex_destroy(&lock);
}
