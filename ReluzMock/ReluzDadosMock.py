import firebase_admin
from firebase_admin import credentials, db
from datetime import datetime, timezone
import random
import time

# Caminho para o arquivo de chave do Firebase
cred = credentials.Certificate("C:/Users/João Lucas/OneDrive/Documentos/Git/Secrets/placassolares-d9d13-firebase-adminsdk-fbsvc-5c68112ce3.json")

# Inicializa o app apenas se ainda não estiver inicializado
if not firebase_admin._apps:
    firebase_admin.initialize_app(cred, {
        'databaseURL': 'https://placassolares-d9d13-default-rtdb.firebaseio.com/'
    })

# IDs simulados
user_id = "userId_1"
panel_id = "Teste"

# Preenche dados de usuário e painel, caso ainda não estejam preenchidos
def setup_users_and_panels():
    print("[INFO] Verificando dados de usuário e painel...")

    user_ref = db.reference(f"users/{user_id}")
    user_ref.set({
        "name": "Joãozinho",
        "email": "joaozin@gugol.com",
        "role": "customer",
        "created_at": "2025-01-01T12:00:00Z"
    })

    panel_ref = db.reference(f"panels/{panel_id}")
    panel_ref.set({
        "owner_id": user_id,
        "location": "UFC",
        "installation_date": "2025-06-24",
        "created_at": "2025-06-24T12:00:00Z",
        "last_updated": datetime.utcnow().replace(tzinfo=timezone.utc).isoformat(),
        "measurements": {}
    })

    print("[INFO] Setup concluído.")

# Gera um timestamp seguro para ser usado como chave no Firebase
def gerar_timestamp_chave():
    return datetime.utcnow().strftime("%Y-%m-%dT%H-%M-%S")

# Simula e envia uma nova medição
def enviar_medida():
    timestamp_iso = datetime.utcnow().replace(tzinfo=timezone.utc).isoformat()
    timestamp_key = gerar_timestamp_chave()

    temperatura = round(random.uniform(24.0, 30.0), 2)
    tensao = round(random.uniform(11.5, 13.0), 2)
    luminosidade = random.randint(300, 1000)

    print(f"[ENVIO] Dados @ {timestamp_iso} -> T: {temperatura}°C | V: {tensao}V | L: {luminosidade}lx")

    medida_ref = db.reference(f"panels/{panel_id}/measurements/{timestamp_key}")
    medida_ref.set({
        "temperature": temperatura,
        "voltage": tensao,
        "luminosity": luminosidade,
        "timestamp": timestamp_iso  # Armazena timestamp completo dentro da medição
    })

    # Atualiza o last_updated do painel
    panel_ref = db.reference(f"panels/{panel_id}/last_updated")
    panel_ref.set(timestamp_iso)

# Execução principal
if __name__ == "__main__":
    setup_users_and_panels()

    for i in range(10):
        enviar_medida()
        time.sleep(5)
