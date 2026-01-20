module dru {
    requires javafx.controls;
    requires javafx.fxml;
    requires com.fazecast.jSerialComm;

    opens dru to javafx.fxml;
    opens dru.ui to javafx.fxml;

    exports dru;
    exports dru.ui;
    exports dru.model;
    exports dru.serial;
}
