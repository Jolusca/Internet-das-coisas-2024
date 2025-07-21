import firebase_admin
from firebase_admin import credentials, db
from datetime import datetime
import matplotlib.pyplot as plt

# Inicializa o Firebase (mesma configuração do seu script)
cred = credentials.Certificate("D:/Arquivos Pessoais João Lucas/SecretsFirebase/placassolares-d9d13-firebase-adminsdk-fbsvc-5f7e4fd3df.json")
if not firebase_admin._apps:
    firebase_admin.initialize_app(cred, {
        'databaseURL': 'https://placassolares-d9d13-default-rtdb.firebaseio.com/'
    })

panel_id = "Teste"

# Pega todos os dados do painel
def pegar_dados_panel(panel_id):
    ref = db.reference(f"panels/{panel_id}/measurements")
    dados = ref.get()
    return dados

# Processa os dados para listas ordenadas por tempo
def processar_dados(dados):
    timestamps = []
    voltagens = []

    for key in sorted(dados.keys()):  # ordena as keys que são timestamp formatadas
        dado = dados[key]
        # tenta pegar o timestamp real (ISO string) dentro do dado, se existir
        ts_str = dado.get("timestamp", key)
        try:
            ts = datetime.strptime(ts_str, "%Y-%m-%dT%H-%M-%S")
        except ValueError:
            # tenta outro formato, ex: 2025-06-24T19:20:00.370337+00:00
            ts = datetime.fromisoformat(ts_str.replace("Z", "+00:00"))

        timestamps.append(ts)
        voltagens.append(dado.get("voltage", 0))

    return timestamps, voltagens

# Plot do gráfico
def plotar_grafico(timestamps, voltagens):
    plt.figure(figsize=(10, 5))
    plt.plot(timestamps, voltagens, marker='o', linestyle='-')
    plt.title('Voltagem do Painel ao Longo do Tempo')
    plt.xlabel('Tempo')
    plt.ylabel('Voltagem (V)')
    plt.grid(True)
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    dados = pegar_dados_panel(panel_id)
    if dados:
        timestamps, voltagens = processar_dados(dados)
        plotar_grafico(timestamps, voltagens)
    else:
        print("Nenhum dado encontrado para o painel.")
