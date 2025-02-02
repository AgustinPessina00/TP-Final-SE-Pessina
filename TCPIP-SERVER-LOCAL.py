# -*- coding: utf-8 -*-
"""
Created on Sun Nov 12 17:33:00 2023
"""

import socket
import sys
import os
import folium
from selenium import webdriver
from datetime import datetime

##################### SETUP #####################################
from selenium.webdriver.chrome.service import Service

chrome_service = Service(executable_path=r'C:\CercoVirtual\chromedriver.exe')
driver = webdriver.Chrome(service=chrome_service)
driver.get("file:///C:\CercoVirtual\seguimiento.html")

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#################################################################

def folium_plot_locations(coord_list, isVirtualFence, coord_virtual_fence):
    if coord_list:
        m = folium.Map(location=coord_list[0][:2], zoom_start=15)
    
    if coord_virtual_fence and (not coord_list):
        m = folium.Map(location=coord_virtual_fence[:2], zoom_start=15)

    if coord_virtual_fence:
        latitude, longitude = coord_virtual_fence[:2]
        timestamp = coord_virtual_fence[2]
        tooltip = f'Hora: {timestamp}'
        folium.Circle(
            location=[latitude, longitude],
            color="orange",
            fill_color="red",
            radius = 300,
            weight=4,
            fill_opacity = 0.9,
            tooltip=tooltip
        ).add_to(m)
    
    if coord_list:
        for i, coord in enumerate(coord_list):
            latitude, longitude = coord[:2]
            timestamp = coord[2]
            tooltip = f'Hora: {timestamp}'
            icon = folium.features.CustomIcon(icon_image='C:/CercoVirtual/cow_icon.png', icon_size=(15, 15))
            # Agregar un marcador en cada ubicación
            folium.Marker(
                location=[latitude, longitude],
                popup=f'<strong>Dispositivo {i+1}</strong><br>Hora: {timestamp}',
                tooltip=tooltip,
                icon=icon
            ).add_to(m)

    # Unir las ubicaciones con flechas
    for i in range(len(coord_list) - 1):
        folium.PolyLine([coord_list[i][:2], coord_list[i+1][:2]], color="blue", weight=2.5, opacity=1).add_to(m)
    
    m.save('C:\CercoVirtual\seguimiento.html')
    driver.refresh()

def main():
    print('Bienvenido')
    print('Cerco Virtual Sistemas embebidos')
    print('Servidor TCP IP - Dispositivo Rastreador GPS para animales')
    
    timeout = 20  # Tiempo límite sin recibir datos en segundos


    ######### INICIANDO SERVIDOR TCP/IP##########
    # CAPITAL: server_address = ('192.168.0.45', 123)
    server_address = ('192.168.1.33', 123)
    print('Iniciando en {} Puerto {}'.format(*server_address))
    sock.bind(server_address)
    sock.listen(1)
    
    coord_list = []  # Lista para almacenar las coordenadas recibidas
    coord_virtual_fence = {}    #Inicializo el cerco.
    
    while True:
        print('Esperando envio de datos')
        connection, client_address = sock.accept()
        connection.settimeout(timeout)  # Establecer el tiempo límite para recibir datos

        try:
            print('Conexion desde rastreador:', client_address)
            
            while True:
                # RECIBE DATOS
                try:
                    data = connection.recv(64)  # Ajusta el tamaño del búfer según tus necesidades
                    if not data:
                        print('No se pudo encontrar más datos desde:', client_address)
                        break
                    data = data.decode()  # Decodifica los datos recibidos
                    device_id, latitude, longitude, isVirtualFence = data.split('|')  # Divide los datos en las variables correspondientes
                    latitude = float(latitude)
                    longitude = float(longitude)
                    print('ID de Dispositivo:', device_id)
                    print('Latitud:', latitude)
                    print('Longitud:', longitude)
                    print('Cerco virtual:', isVirtualFence)
                    isVirtualFence = isVirtualFence.strip() == 'True'

                    # PLOTEO DE COORDENADAS
                    timestamp = datetime.now().strftime("%H:%M:%S")
                    if isVirtualFence:
                        coord_virtual_fence = (latitude, longitude, timestamp)
                    else:
                        coord_list.append((latitude, longitude, timestamp))

                    folium_plot_locations(coord_list, isVirtualFence, coord_virtual_fence)

                except socket.timeout:
                    print('No se recibieron datos durante {} segundos. Cerrando conexión.'.format(timeout))
                    break

        finally:
            print('Cerrando conexion TCP/IP')
            connection.close()


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print('Interrupted')
        try:
            sys.exit(0)
        except SystemExit:
            os._exit(0)
