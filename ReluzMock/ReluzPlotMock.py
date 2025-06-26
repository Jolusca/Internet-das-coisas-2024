import firebase_admin
from firebase_admin import credentials, db
from datetime import datetime
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from scipy.interpolate import make_interp_spline
import numpy as np

# Inicializa o Firebase
cred = credentials.Certificate("C:/Users/João Lucas/OneDrive/Documentos/Git/Secrets/placassolares-d9d13-firebase-adminsdk-fbsvc-5c68112ce3.json")
if not firebase_admin._apps:
    firebase_admin.initialize_app(cred, {
        'databaseURL': 'https://placassolares-d9d13-default-rtdb.firebaseio.com/'
    })

panel_id = "Teste"

def pegar_dados_panel(panel_id):
    ref = db.reference(f"panels/{panel_id}/measurements")
    dados = ref.get()
    return dados

def processar_dados(dados):
    timestamps = []
    voltagens = []

    for key in sorted(dados.keys()):
        dado = dados[key]
        ts_str = dado.get("timestamp", key)
        try:
            ts = datetime.strptime(ts_str, "%Y-%m-%dT%H-%M-%S")
        except ValueError:
            ts = datetime.fromisoformat(ts_str.replace("Z", "+00:00"))

        timestamps.append(ts)
        voltagens.append(dado.get("voltage", 0))

    return timestamps, voltagens

def plotar_grafico(timestamps, voltagens):
    plt.figure(figsize=(10, 5))

    # Converter timestamps para números (necessário para spline)
    x = mdates.date2num(timestamps)
    y = np.array(voltagens)

    # Suavização com spline (se houver dados suficientes)
    if len(x) >= 4:
        x_new = np.linspace(x.min(), x.max(), 300)
        spline = make_interp_spline(x, y, k=3)
        y_smooth = spline(x_new)
        plt.plot_date(x_new, y_smooth, '-', label="Voltagem Suavizada", color='tab:blue')
    else:
        # fallback simples sem suavização
        plt.plot(timestamps, voltagens, marker='o', linestyle='-', label="Voltagem")

    plt.title('Voltagem do Painel ao Longo do Tempo')
    plt.xlabel('Tempo')
    plt.ylabel('Voltagem (V)')
    plt.grid(True)
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.legend()
    plt.show()

if __name__ == "__main__":
    dados = pegar_dados_panel(panel_id)
    if dados:
        timestamps, voltagens = processar_dados(dados)
        plotar_grafico(timestamps, voltagens)
    else:
        print("Nenhum dado encontrado para o painel.")
