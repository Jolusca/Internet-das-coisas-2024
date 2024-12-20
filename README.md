## Comando para ler as mensagens na plataforma azure pela linha de comando no bash

az iot hub monitor-events --hub-name <NomeDoSeuIoTHub> --device-id <DeviceId> --properties all

### Para aparecer apenas a payload
az iot hub monitor-events --hub-name TesteIotJlucas2002 --device-id TesteIotJL --properties all | grep '"payload"'

usando a chave principal no CONNECTIONSTRING 


##Doc do projeto
https://docs.google.com/document/d/1ZLZTUfdrXvg6JXVGr9zEuLp75r-rQKLy/edit
