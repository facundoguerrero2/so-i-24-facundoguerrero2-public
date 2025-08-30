/**
 * @file main.c
 * @brief Entry point of the system
 */
#include "expose_metrics.h"
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>
/**
 * @brief Ruta fifo configs
 */
#define FIFO_CONFIG "/tmp/fifo_config"
/**
 * @brief ruta fifo config ack
 */
#define FIFO_CONFIG_ACK "/tmp/fifo_config_ack"
/**
 * @brief Size of cfg array
 */
#define CONFIG_SIZE 11

/**
 * @brief Main function to initialize and update metrics.
 *
 * This function initializes the metrics, creates a thread to expose
 * the metrics via HTTP, and enters a loop to update the metrics every second.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int Returns EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */

int main(int argc, char* argv[])
{
    int fd_config, fd_config_ack;
    init_metrics();
    // Creamos un hilo para exponer las métricas vía HTTP
    int cfg[CONFIG_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

    fd_config = open(FIFO_CONFIG, O_RDONLY);

    read(fd_config, cfg, sizeof(cfg));

    fd_config_ack = open(FIFO_CONFIG_ACK, O_WRONLY);
    write(fd_config_ack, cfg, sizeof(cfg));

    pthread_t tid;
    if (pthread_create(&tid, NULL, expose_metrics, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo del servidor HTTP\n");
        return EXIT_FAILURE;
    }

    // Bucle principal para actualizar las métricas cada segundo
    while (true)
    {
        if (cfg[0] == 1)
        {
            update_cpu_gauge();
        }
        if (cfg[1] == 1)
        {
            update_memory_gauge();
        }
        if (cfg[2] == 1)
        {
            update_disk_avg_read_time();
        }
        if (cfg[3] == 1)
        {
            update_disk_avg_write_time();
        }
        if (cfg[4] == 1)
        {
            update_disk_IO_percentage_time();
        }
        if (cfg[5] == 1)
        {
            update_net_receive_kbps();
        }
        if (cfg[6] == 1)
        {
            update_net_sent_kbps();
        }
        if (cfg[7] == 1)
        {
            update_net_received_packets();
        }
        if (cfg[8] == 1)
        {
            update_net_sent_packets();
        }
        if (cfg[9] == 1)
        {
            update_processes_gauge();
            update_fragmentation_gauge();
            update_political_fit_counters();
        }
        sleep(cfg[10]);
    }

    return EXIT_SUCCESS;
}
