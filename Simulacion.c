#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#define HV 9999

t_list* list_llegadas;

typedef struct{
	float tiempo_evento;
} evento;

typedef struct{
	float tiempo_salida;
	float total_tiempo_ocioso;
	float inicio_tiempo_ocioso;
}server;

server *server_des;
server *server_sop;

void inicializar_servers(int32_t M, int32_t N){
	int i;
	server_des = malloc(M*sizeof(server));
	server_sop = malloc(N*sizeof(server));

	for(i=0; i< M; i++){
		server_des[i].tiempo_salida = HV;
		server_des[i].total_tiempo_ocioso = 0;
		server_des[i].inicio_tiempo_ocioso = 0;
	}

	for(i=0; i< N; i++){
		server_sop[i].tiempo_salida = HV;
		server_sop[i].total_tiempo_ocioso = 0;
		server_sop[i].inicio_tiempo_ocioso = 0;
	}
}

float random_value(){
	float x = ((float)rand()/(float)(RAND_MAX));
	return x;
}

bool distribucion_cola(){
	bool resultado;
	float r = random_value();
	if(r <= 0.2){
		resultado = true;
	}else{
		resultado = false;
	}
	return resultado;
}

int proxima_salida_des(int size){
	int i, posicion = 0;
	float menor = HV;

	for(i=0; i<size; i++){
		if((server_des[i].tiempo_salida) <= menor){
			posicion = i;
			menor = server_des[i].tiempo_salida;
		}
	}
	return posicion;
}

int proxima_salida_sop(int size){
	int i, posicion = 0;
	float menor = HV;

	for(i=0; i<size; i++){
		if((server_sop[i].tiempo_salida) <= menor){
			posicion = i;
			menor = server_sop[i].tiempo_salida;
		}
	}
	return posicion;
}

int proxima_llegada(){
	int i, posicion, size = list_size(list_llegadas);
	float menor = HV;

	for(i=0; i<size; i++){
		evento *prox_llegada = list_get(list_llegadas, i);
		if(prox_llegada->tiempo_evento < menor){
			menor = prox_llegada->tiempo_evento;
			posicion = i;
		}
	}
	return posicion;
}

float generar_intervalo_llegada(){
	float r = random_value();
	return 1 + r*11;
}

float generar_tiempo_atencion_d(){
	float r = random_value();
	return 3 + r*5;
}

float generar_tiempo_atencion_s(){
	float r = random_value();
	return 0.5 + r*0.5;
}

int server_des_mas_ocioso(int size, float tiempo){
	int i, posicion = 0;
	float mayor = 0;

	for(i=0; i<size; i++){
		if((tiempo - server_des[i].inicio_tiempo_ocioso) > mayor){
			posicion = i;
			mayor = tiempo - server_des[i].inicio_tiempo_ocioso;
		}
	}
	return posicion;
}

int server_sop_mas_ocioso(int size, float tiempo){
	int i, posicion = 0;
	float mayor = 0;

	for(i=0; i<size; i++){
		if((tiempo - server_sop[i].inicio_tiempo_ocioso) > mayor){
			posicion = i;
			mayor = tiempo - server_sop[i].inicio_tiempo_ocioso;
		}
	}
	return posicion;
}

int main(int argc, char** argv){
	system("clear");
	printf("SISTEMA DE SIMULACION DE COLAS DE TICKETS, by GIUSSAyCo. Ltd.\nAll rights reserved\n\nPulse cualquier tecla para continuar\n");
	getchar();
	system("clear");

	list_llegadas = list_create();

	int32_t *cant_serv_des = malloc(sizeof(int32_t));
	int32_t *cant_serv_sop = malloc(sizeof(int32_t));

	int32_t numero_tickets_des = 0;
	int32_t numero_tickets_sop = 0;

	float total_atencion_desarrollo = 0;
	float total_atencion_soporte = 0;

	float total_llegadas_d = 0;
	float total_llegadas_s = 0;
	float total_salidas_d = 0;
	float total_salidas_s = 0;

	int32_t total_operaciones_desarrollo = 0;
	int32_t total_operaciones_soporte = 0;

	printf("Ingrese numero de servidores de Desarrollo: ");
	scanf("%d", cant_serv_des);
	system("clear");
	printf("Ingrese numero de servidores de Soporte: ");
	scanf("%d", cant_serv_sop);
	system("clear");

	inicializar_servers(*cant_serv_des, *cant_serv_sop);

	float tiempo = 0;
	float tiempo_final = 120;

	evento *primera_llegada = malloc(sizeof(evento));
	primera_llegada->tiempo_evento = 0;
	list_add(list_llegadas, primera_llegada);

	while(tiempo <= tiempo_final){
		int menor_d = proxima_salida_des(*cant_serv_des), menor_s = proxima_salida_sop(*cant_serv_sop);
		int menor_ll = proxima_llegada();
		evento *prox_llegada = list_get(list_llegadas, menor_ll);

		if(prox_llegada->tiempo_evento <= server_des[menor_d].tiempo_salida && prox_llegada->tiempo_evento <= server_sop[menor_s].tiempo_salida){
			evento *evento_sucedido = list_remove(list_llegadas, menor_ll);
			tiempo = evento_sucedido->tiempo_evento;

			printf("\nLlegada en %f\n", tiempo);

			float intervalo_arribo = generar_intervalo_llegada();
			evento *nueva_llegada = malloc(sizeof(evento));
			nueva_llegada->tiempo_evento = tiempo + intervalo_arribo;
			list_add(list_llegadas, nueva_llegada);
			free(evento_sucedido);

			bool tipo = distribucion_cola();
			if(tipo == 1){
				numero_tickets_des ++;
				printf("Nuevo ticket de Desarrollo\n");
				if(numero_tickets_des <= *cant_serv_des){
					int server_mas_ocioso = server_des_mas_ocioso(*cant_serv_des, tiempo);
					float tiempo_atencion = generar_tiempo_atencion_d();

					total_atencion_desarrollo += tiempo_atencion;
					server_des[server_mas_ocioso].tiempo_salida = tiempo + tiempo_atencion;
					server_des[server_mas_ocioso].total_tiempo_ocioso = tiempo - server_des[server_mas_ocioso].inicio_tiempo_ocioso;

					printf("Habra nueva salida de desarrollo en %f\n", tiempo + tiempo_atencion);
				}
				total_llegadas_d += tiempo;
			}else{
				numero_tickets_sop ++;
				printf("Nuevo ticket de Soporte\n");
				if(numero_tickets_sop <= *cant_serv_sop){
					int server_mas_ocioso = server_sop_mas_ocioso(*cant_serv_sop, tiempo);
					float tiempo_atencion = generar_tiempo_atencion_s();

					total_atencion_soporte += tiempo_atencion;
					server_sop[server_mas_ocioso].tiempo_salida = tiempo + tiempo_atencion;
					server_sop[server_mas_ocioso].total_tiempo_ocioso = tiempo - server_sop[server_mas_ocioso].inicio_tiempo_ocioso;

					printf("Habra nueva salida de soporte en %f\n", tiempo + tiempo_atencion);
			}
				total_llegadas_s += tiempo;
			}
		}else if(server_des[menor_d].tiempo_salida <= server_sop[menor_d].tiempo_salida){
			tiempo = server_des[menor_d].tiempo_salida;
			printf("\nSalida en %f\n", tiempo);

			numero_tickets_des --;
			if(numero_tickets_des >= *cant_serv_des){
				float tiempo_atencion = generar_tiempo_atencion_d();
				server_des[menor_d].tiempo_salida = tiempo + tiempo_atencion;
				total_atencion_desarrollo += tiempo_atencion;

				printf("Habra nueva salida de desarrollo en %f\n", tiempo + tiempo_atencion);
			}else{
				server_des[menor_d].inicio_tiempo_ocioso = tiempo;
				server_des[menor_d].tiempo_salida = HV;
			}
			total_operaciones_desarrollo ++;
			total_salidas_d += tiempo;
		}else{
			tiempo = server_sop[menor_s].tiempo_salida;
			printf("\nSalida en %f\n", tiempo);

			numero_tickets_sop --;
			if(numero_tickets_sop >= *cant_serv_sop){
				float tiempo_atencion = generar_tiempo_atencion_s();
				server_sop[menor_s].tiempo_salida = tiempo + tiempo_atencion;
				total_atencion_soporte += tiempo_atencion;

				printf("Habra nueva salida de soporte en %f\n", tiempo + tiempo_atencion);
			}else{
				server_sop[menor_s].inicio_tiempo_ocioso = tiempo;
				server_sop[menor_s].tiempo_salida = HV;
			}
			total_operaciones_soporte ++;
			total_salidas_s += tiempo;
		}
		sleep(1);
	}

	float promedio_tiempo_espera_des = (total_salidas_d - total_llegadas_d - total_atencion_desarrollo)/total_operaciones_desarrollo;
	float promedio_tiempo_espera_sop = (total_salidas_s - total_llegadas_s - total_atencion_soporte)/total_operaciones_soporte;

	printf("\n*****Fin de Simulacion*****\nResultados: PECD= %f,PECS= %f\n", promedio_tiempo_espera_des, promedio_tiempo_espera_sop);

	free(server_des);
	free(server_sop);
	free(cant_serv_des);
	free(cant_serv_sop);
	getchar();
	return 0;
}
