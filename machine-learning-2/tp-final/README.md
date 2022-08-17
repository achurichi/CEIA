# Machine Learning 2 Trabajo Práctico Final

Alumnos: Alan Churichi, Juan Pabalo Alianak

## Caso de Negocio

Las personas son el mayor activo que tiene una empresa, gestionar los recursos
humanos de manera efectiva resulta imprescindible para evitar su fuga.

El modelo planteado busca ser consumido por el área de recursos humanos de medianas y grandes empresas, de manera de poder analizar en simultaneo un gran numero de empleados y trabajar de forma personalizada con aquellos que el modelo detecta como desgastados.

## Arquitectura

![diagrama de arquitectura](diagram.svg 'Title')

### Google Cloud

Toda la infraestructura de nuestra solución se desarrolla en google cloud.

### Base de Datos

Base de datos PostgreSQL. Tiene tres funciones principales:

- Almacenar los datos de entrenamiento para los modelos de ML.
- Almacenar información sobre los modelos generados por MLflow. Como por ejemplo: ids, versiones, estado, etc.
- Almacenar los resultados de las predicciones.

### Bucket

Bucket en Google Cloud para almacenar los artifacts de los modelos entrenados.

### VM Training

Maquina Virtual que se utiliza para entrenar el modelo. Contiene distintos archivos `.py` en los que se realiza el entrenamiento del modelo, se registra la metadata del mismo en la Base de Datos utilizando MLflow y se almacena el modelo en un bucket.
El punto de entrada para realizar el entrenamiento es el archivo `training_pipeline.sh` que se encarga de descargar los datos para utilizar en el entrenamiento de la Base de Datos, ejecutar el archivo `train_model.py` para generar el modelo y borrar los datos cuando ya no son necesarios.

Características:

- CPU Cores: 2
- RAM: 8Gb
- Almacenamiento: 20Gb SSD
- SO: Ubuntu 20.04 x86/64

### VM Serving

Maquina Virtual que se utiliza para generar inferencias. Se utiliza MLflow para conectarse a la Base de Datos y descargar la metadata de los modelos y versiones. Con esta información se descargan, del bucket de Google Cloud, los artifacts del modelo tageado en `Production`. Finalmente se sirve el modelo con ayuda de MLflow.
El punto de entrada es el archivo `screen.sh` que genera una nueva screen y realiza los pasos mencionados anteriormente para servir el modelo.

Características:

- CPU Cores: 2
- RAM: 2Gb
- Almacenamiento: 10Gb HDD
- SO: Ubuntu 20.04 x86/64

### Cloud Function

Se utiliza una Cloud Function para poder hacer inferencias de manera remota. Esta función recibe la información necesaria para realizar una predicción, se comunica con la VM que sirve el modelo, obtiene los resultados, los almacena en la Base de Datos y finalmente los devuelve al usuario que realizó la petición.

Ejemplo:

```
curl -m 15 -X POST https://us-west2-ceia-machine-learning-2.cloudfunctions.net/ml2-external-inference \
-H "Content-Type:application/json" \
-d '{
	"user_id": 1,
	"data": [
		{
			"age": 40,
			"distancefromhome": 4,
			"education": 3,
			"environmentsatisfaction": 1,
			"joblevel": 2,
			"jobsatisfaction": 3,
			"maritalstatussingle": 1,
			"monthlyincome": 4000,
			"numcompaniesworked": 2,
			"overtime": 0,
			"percentsalaryhike": 11,
			"relationshipsatisfaction": 4,
			"stockoptionlevel": 0,
			"totalworkingyears": 10,
			"trainingtimeslastyear": 2,
			"worklifebalance": 3,
			"yearsatcompany": 6,
			"yearsincurrentrole": 3,
			"yearssincelastpromotion": 2,
			"yearswithcurrmanager": 2
		}
	]
}'
```

### Aplicación Web (no implementado)

Aplicación Web donde el usuario puede cargar los datos requeridos para realizar una predicción y luego ver el resultado. Se comunica con la Cloud Function.

## Futuras Mejoras

- Implementar una UI para que el modelo sea consumido de manera organica.
- Guardar métricas de entrenamiento.
- Generar un ambiente de staging para probar los modelos.
- Podes servir la UI de Mlflow de manera remota.
- Automatizar el deploy de modelos de entrenamiento.
- Encender la VM de entrenamiento a intervalos predefinidos para ahorrar recursos.

## Conclusiones

Con la solucion implementada, el area de recursos humanos de una empresa puede generar inferencias en base a los datos de sus empleados y analizar el desgaste de ellos.

Logramos servir un modelo en produccion utilizando Google Cloud como proveedor de servicios en la nube.

El modelo puede ser accedido desde cualquier computadora conectada a internet, generar inferencias y almacenar los resultados.

La arquitectura diseñada es escalable, adaptandose a cambios de tipo y/o tamaño de modelos.