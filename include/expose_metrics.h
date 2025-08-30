/**
 * @file expose_metrics.h
 * @brief Programa para leer el uso de CPU y memoria y exponerlos como métricas de Prometheus.
 * This file contains the necessary includes and declarations for exposing metrics.
 */

#include "metrics.h"
#include <errno.h>
#include <prom.h>
#include <promhttp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para sleep

/**
 * @brief tamaño de buffer para guardar las lineas al leerlas
 */
#define BUFFER_SIZE 256

/**
 * @brief Actualiza la métrica de uso de CPU.
 */
double update_cpu_gauge();

/**
 * @brief Actualiza la métrica de cantidad de procesos.
 */
double update_processes_gauge();
/**
 * @brief Actualiza la métrica de uso de memoria.
 */
double update_memory_gauge();

/**
 * @brief Actualiza la métrica tiempo promedio de lectura
 */
double update_disk_avg_read_time();

/**
 * @brief Actualiza la métrica tiempo promedio de escritura
 */
double update_disk_avg_write_time();

/**
 * @brief Actualiza la métrica de porcentaje de uso del disco
 */
double update_disk_IO_percentage_time();

/**
 * @brief Actualiza la métrica kbps recibidos
 */
double update_net_receive_kbps();

/**
 * @brief Actualiza la métrica kbps enviados
 */
double update_net_sent_kbps();

/**
 * @brief Actualiza la métrica paquetes recibidos
 */
unsigned long long update_net_received_packets();

/**
 * @brief Actualiza la métrica paquetes enviados
 */
unsigned long long update_net_sent_packets();

/**
 * @brief Actualiza la métrica porcentaje de fragmentacion
 */
double update_fragmentation_gauge();

/**
 * @brief Actualiza los contadores de first fit best fit y worst fit 
 */
unsigned long long update_political_fit_counters();

/**
 * @brief Función del hilo para exponer las métricas vía HTTP en el puerto 8000.
 * @param arg Argumento no utilizado.
 * @return NULL
 */
void* expose_metrics(void* arg);

/**
 * @brief Inicializar mutex y métricas.
 */
int init_metrics();

/**
 * @brief Destructor de mutex
 */
void destroy_mutex();
