import random
import time
from firebase_admin import credentials, initialize_app, db

# Configuração do Firebase
cred = credentials.Certificate("C:\\Users\\João Lucas\\OneDrive\\Área de Trabalho\\gesa-iot-firebase-adminsdk-fbsvc-9eb28b67bc.json")  # Insira o caminho para seu arquivo JSON
initialize_app(cred, {"databaseURL": "https://gesa-iot-default-rtdb.firebaseio.com"})

# Parâmetros configuráveis
initial_timestamp = 1090  # Timestamp inicial
increment = 5  # Incremento no timestamp

# Referência ao nó no Firebase
ref = db.reference("sensores2")

# Estados e suas configurações
states = [
    {"name": "Luz Total", "iterations": 20, "ldr_values": [1, 1, 1, 1], "voltage_range": (12.5, 13), "temperature_range": (39, 40)},
    {"name": "Sombra Direita", "iterations": 15, "ldr_values": [0, 0, 1, 1], "voltage_range": (8, 8.5), "temperature_range": (39, 40)},
    {"name": "Luz Total", "iterations": 10, "ldr_values": [1, 1, 1, 1], "voltage_range": (12.5, 13), "temperature_range": (39, 40)},
    {"name": "Sombra Esquerda", "iterations": 17, "ldr_values": [1, 1, 0, 0], "voltage_range": (8.5, 9), "temperature_range": (39, 40)},
    {"name": "Luz Total", "iterations": 7, "ldr_values": [1, 1, 1, 1], "voltage_range": (12.5, 13), "temperature_range": (39, 40)},
    {"name": "Sombra Total", "iterations": 15, "ldr_values": [0, 0, 0, 0], "voltage_range": (2.5, 3), "temperature_range": (37, 37.7)},
]

def generate_data(voltage_range, temperature_range):
    # Gera valores de tensão e temperatura aleatórios dentro dos intervalos definidos
    voltage = round(random.uniform(*voltage_range), 2)
    temperature = round(random.uniform(*temperature_range), 2)
    return voltage, temperature

def push_data():
    global initial_timestamp
    for state in states:
        print(f"Estado atual: {state['name']}")
        for _ in range(state["iterations"]):
            voltage, temperature = generate_data(state["voltage_range"], state["temperature_range"])
            data = {
                "ldr1": state["ldr_values"][0],
                "ldr2": state["ldr_values"][1],
                "ldr3": state["ldr_values"][2],
                "ldr4": state["ldr_values"][3],
                "voltage": voltage,
                "temperature": temperature,
                "timestamp": initial_timestamp,
            }
            ref.push(data)  # Envia os dados ao Firebase
            print(f"Dado enviado: {data}")
            initial_timestamp += increment
            time.sleep(1)  # Intervalo entre envios (opcional)

# Executar o push dos dados
push_data()