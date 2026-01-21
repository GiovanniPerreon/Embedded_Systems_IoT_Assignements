package dru;

import dru.ui.DroneRemoteController;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

/**
 * Main application entry point for Drone Remote Unit.
 *
 * This JavaFX application provides a GUI for controlling the
 * Smart Drone Hangar system via serial communication.
 *
 * Features:
 * - Connect to Arduino via serial port
 * - Send TAKEOFF and LAND commands
 * - Display drone state (REST, TAKEOFF, OPERATING, LANDING)
 * - Display hangar state (NORMAL, PREALARM, ALARM)
 * - Show landing distance during landing phase
 * - Handle alarm notifications
 */
public class DroneRemoteUnit extends Application {

    private DroneRemoteController controller;

    @Override
    public void start(Stage primaryStage) throws Exception {
        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/dru/ui/DroneRemoteView.fxml")
        );
        Parent root = loader.load();
        controller = loader.getController();

        Scene scene = new Scene(root, 500, 750);

        primaryStage.setTitle("Drone Remote Unit - Smart Drone Hangar");
        primaryStage.setScene(scene);
        primaryStage.setMinWidth(450);
        primaryStage.setMinHeight(650);
        primaryStage.setOnCloseRequest(e -> {
            if (controller != null) {
                controller.shutdown();
            }
        });

        primaryStage.show();
    }

    @Override
    public void stop() {
        if (controller != null) {
            controller.shutdown();
        }
    }

    public static void main(String[] args) {
        launch(args);
    }
}
