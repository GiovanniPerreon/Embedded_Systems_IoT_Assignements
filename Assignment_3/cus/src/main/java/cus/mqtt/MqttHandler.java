package cus.mqtt;

import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import cus.config.Config;
import cus.model.SystemState;
import org.eclipse.paho.client.mqttv3.*;

public class MqttHandler {

    private MqttClient client;
    private final SystemState state;

    public MqttHandler(SystemState state) {
        this.state = state;
    }

    public void connect() throws MqttException {
        client = new MqttClient(Config.MQTT_BROKER, Config.MQTT_CLIENT_ID);

        MqttConnectOptions options = new MqttConnectOptions();
        options.setAutomaticReconnect(true);
        options.setCleanSession(true);
        options.setConnectionTimeout(10);

        client.setCallback(new MqttCallback() {
            @Override
            public void connectionLost(Throwable cause) {
                System.err.println("[MQTT] Connection lost: " + cause.getMessage());
            }

            @Override
            public void messageArrived(String topic, MqttMessage message) {
                handleMessage(topic, new String(message.getPayload()));
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {}
        });

        client.connect(options);
        client.subscribe(Config.MQTT_TOPIC, Config.MQTT_QOS);
        System.out.println("[MQTT] Connected to " + Config.MQTT_BROKER
            + ", subscribed to " + Config.MQTT_TOPIC);
    }

    private void handleMessage(String topic, String payload) {
        try {
            JsonObject json = JsonParser.parseString(payload).getAsJsonObject();
            double level = json.get("level").getAsDouble();
            state.addWaterLevel(level);
            System.out.println("[MQTT] Water level: " + level + " cm");
        } catch (Exception e) {
            System.err.println("[MQTT] Failed to parse: " + payload + " -- " + e.getMessage());
        }
    }

    public void disconnect() {
        try {
            if (client != null && client.isConnected()) {
                client.disconnect();
                client.close();
            }
        } catch (MqttException e) {
            System.err.println("[MQTT] Error disconnecting: " + e.getMessage());
        }
    }

    public boolean isConnected() {
        return client != null && client.isConnected();
    }
}
