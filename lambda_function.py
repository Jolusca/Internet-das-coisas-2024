import json
import boto3

client = boto3.client('iot-data', region_name='us-east-2')

def lambda_handler(event, context):
    print(event)  # Logs para ver o evento de entrada
    # Publicar mensagem no tópico 'esp32/sub'
    response = client.publish(
        topic='esp32/sub',  # O tópico MQTT onde você quer publicar a mensagem
        qos=1,               # Qualidade de serviço
        payload=json.dumps({"foo": "bar"})  # Mensagem a ser enviada
    )
    print(response)  # Log da resposta da publicação

    return {
        'statusCode': 200,
        'body': json.dumps('Mensagem publicada no tópico esp32/sub')
    }
import json
import boto3

# Configuração do cliente IoT
iot_client = boto3.client('iot-data')

def lambda_handler(event, context):
    # Extrai os valores dos LDRs e temperatura do evento
    ldr1 = event['LDR1']
    ldr2 = event['LDR2']
    ldr3 = event['LDR3']
    ldr4 = event['LDR4']
    
    # Lógica para determinar a direção
    if (ldr1 == 0 or ldr2 == 0) and ldr3 == 1 and ldr4 == 1:
        message = "Direita"
    elif (ldr3 == 0 or ldr4 == 0) and ldr1 == 1 and ldr2 == 1:
        message = "Esquerda"
    elif sum([ldr1, ldr2, ldr3, ldr4]) == 1:
        if ldr1 == 1:
            message = "Esquerda"
        elif ldr4 == 1:
            message = "Direita"
    else:
        # Caso não se enquadre em nenhuma das condições acima, não publica nada
        return
    
    # Publica a mensagem no tópico esp32/sub
    response = iot_client.publish(
        topic='esp32/sub',
        qos=1,
        payload=json.dumps({"message": message})
    )
    
    return {
        'statusCode': 200,
        'body': json.dumps('Mensagem publicada com sucesso!')
    }