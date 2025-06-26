import firebase_admin
from firebase_admin import credentials, db
import pandas as pd
import matplotlib.pyplot as plt

# Configuração do Firebase
cred = credentials.Certificate("C:\\Users\\João Lucas\\OneDrive\\Área de Trabalho\\gesa-iot-firebase-adminsdk-fbsvc-9eb28b67bc.json")  # Insira o caminho para seu arquivo JSON
firebase_admin.initialize_app(cred, {"databaseURL": "https://gesa-iot-default-rtdb.firebaseio.com"})

# Referência ao nó no Firebase
ref = db.reference("sensores")

# Função para buscar dados do Firebase
def fetch_data():
    data = ref.get()
    if not data:
        print("Nenhum dado encontrado no Firebase.")
        return None
    
    # Convertendo dados em DataFrame
    df = pd.DataFrame(data.values())
    df = df.sort_values(by="timestamp")  # Ordena por timestamp
    return df

# Função para aplicar média móvel (suavização leve)
def smooth_moving_average(series, window_size=3):
    return series.rolling(window=window_size, min_periods=1).mean()

# Função para plotar gráficos
def plot_graphs(df):
    if df is None:
        return

    # Aplicar suavização leve na temperatura
    df["temperature_smooth"] = smooth_moving_average(df["temperature"], window_size=3)

    # Gráfico de Tensão por Timestamp (Dados brutos)
    plt.figure(figsize=(10, 5))
    plt.plot(df["timestamp"], df["voltage"], marker="o", label="Tensão (V)", color="blue")
    plt.xlabel("Timestamp")
    plt.ylabel("Tensão (V)")
    plt.title("Tensão por Timestamp")
    plt.ylim(df["voltage"].min() - 0.5, df["voltage"].max() + 0.5)  # Limite ajustado
    plt.legend()
    plt.grid()
    plt.show()

    # Gráfico dos LDRs por Timestamp (Dados brutos)
    plt.figure(figsize=(10, 5))
    for i in range(1, 5):
        plt.plot(df["timestamp"], df[f"ldr{i}"], marker="o", label=f"LDR{i}")
    plt.xlabel("Timestamp")
    plt.ylabel("LDR (0 ou 1)")
    plt.title("LDRs por Timestamp")
    plt.ylim(-0.2, 1.2)  # Limite ajustado para LDRs
    plt.legend()
    plt.grid()
    plt.show()

    # Gráfico de Temperatura por Timestamp (Apenas suavizado, com linha e pontos)
    plt.figure(figsize=(10, 5))
    plt.plot(df["timestamp"], df["temperature_smooth"], marker="o", label="Temperatura (°C) - Suavizado", color="red")
    plt.xlabel("Timestamp")
    plt.ylabel("Temperatura (°C)")
    plt.title("Temperatura por Timestamp")
    plt.ylim(df["temperature"].min() - 2, df["temperature"].max() + 2)  # Limite ajustado
    plt.legend()
    plt.grid()
    plt.show()

    # Gráfico dos LDRs e Tensão por Timestamp (Dados brutos)
    plt.figure(figsize=(12, 6))
    for i in range(1, 5):
        plt.plot(df["timestamp"], df[f"ldr{i}"], marker="o", label=f"LDR{i}")
    plt.plot(df["timestamp"], df["voltage"], marker="s", label="Tensão (V)", linestyle="--", color="purple")
    plt.xlabel("Timestamp")
    plt.ylabel("Valores")
    plt.title("LDRs e Tensão por Timestamp ")
    plt.legend()
    plt.grid()
    plt.show()

# Buscar dados e plotar gráficos
data_frame = fetch_data()
plot_graphs(data_frame)