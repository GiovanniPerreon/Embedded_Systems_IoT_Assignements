module dru {
    requires transitive javafx.controls;
    requires transitive javafx.fxml;
    requires transitive javafx.graphics;
    requires transitive javafx.base;
    requires com.fazecast.jSerialComm;

    opens dru to javafx.fxml;
    opens dru.ui to javafx.fxml;
    opens dru.model to javafx.base;

    exports dru;
    exports dru.ui;
    exports dru.model;
    exports dru.serial;
}
