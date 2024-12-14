Comando para ler as mensagens na plataforma azure pela linha de comando no bash

az iot hub monitor-events --hub-name --device-id --properties all
Para aparecer apenas a payload

az iot hub monitor-events --hub-name TesteIotJlucas2002 --device-id TesteIotJL --properties all | grep '"payload"'

usando a chave principal no CONNECTIONSTRING