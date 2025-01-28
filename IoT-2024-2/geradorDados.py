import random
import time
from firebase_admin import credentials, initialize_app, db

# Configuração do Firebase
cred = credentials.Certificate("C:\\Users\\joaomota\\Desktop\\gesa-iot-firebase-adminsdk-fbsvc-9eb28b67bc.json")  # Insira o caminho para seu arquivo JSON
initialize_app(cred, {"databaseURL": "https://gesa-iot-default-rtdb.firebaseio.com"})

# Parâmetros configuráveis
ldr_values = [0, 0, 0, 0]  # Valores dos LDRs, altere conforme necessário
voltage_range = (1.2, 2.5)  # Faixa de valores para a tensão (em volts)
temperature_range = (34, 35.0)  # Faixa de valores para a temperatura (em graus Celsius)
initial_timestamp = 1090  # Timestamp inicial
increment = 10  # Incremento no timestamp
iterations = 9  # Número de iterações

# Referência ao nó no Firebase
ref = db.reference("sensores")

def generate_data():
    # Gera valores de tensão e temperatura aleatórios dentro dos intervalos definidos
    voltage = round(random.uniform(*voltage_range), 2)
    temperature = round(random.uniform(*temperature_range), 2)
    return voltage, temperature

def push_data():
    global initial_timestamp
    for _ in range(iterations):
        voltage, temperature = generate_data()
        data = {
            "ldr1": ldr_values[0],
            "ldr2": ldr_values[1],
            "ldr3": ldr_values[2],
            "ldr4": ldr_values[3],
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
