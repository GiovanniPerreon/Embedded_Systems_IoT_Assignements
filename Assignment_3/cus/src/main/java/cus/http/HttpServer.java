package cus.http;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonObject;
import cus.config.Config;
import cus.model.SystemMode;
import cus.model.SystemState;
import cus.serial.SerialHandler;
import io.javalin.Javalin;

public class HttpServer {

    private final SystemState state;
    private final SerialHandler serial;
    private final Gson gson;
    private Javalin app;

    public HttpServer(SystemState state, SerialHandler serial) {
        this.state = state;
        this.serial = serial;
        this.gson = new GsonBuilder().create();
    }

    public void start() {
        app = Javalin.create(config -> {
            config.bundledPlugins.enableCors(cors -> {
                cors.addRule(rule -> rule.anyHost());
            });
        });

        setupRoutes();
        app.start(Config.HTTP_PORT);
        System.out.println("[HTTP] Server started on port " + Config.HTTP_PORT);
    }

    private void setupRoutes() {

        // GET /api/state
        app.get("/api/state", ctx -> {
            SystemState.StateSnapshot snap = state.snapshot();

            JsonObject response = new JsonObject();
            response.addProperty("mode", snap.mode().getLabel());
            response.addProperty("valveOpenPercent", snap.valveOpenPercent());
            response.addProperty("latestWaterLevel", snap.latestWaterLevel());
            response.add("waterLevelHistory", gson.toJsonTree(
                snap.waterLevelHistory().stream()
                    .map(r -> {
                        JsonObject reading = new JsonObject();
                        reading.addProperty("level", r.level());
                        reading.addProperty("timestamp", r.timestamp());
                        return reading;
                    })
                    .toList()
            ));

            ctx.contentType("application/json");
            ctx.result(gson.toJson(response));
        });

        // POST /api/mode
        app.post("/api/mode", ctx -> {
            JsonObject body = gson.fromJson(ctx.body(), JsonObject.class);
            String modeStr = body.get("mode").getAsString().toUpperCase();

            JsonObject response = new JsonObject();

            try {
                SystemMode requestedMode = SystemMode.valueOf(modeStr);

                if (requestedMode == SystemMode.UNCONNECTED) {
                    ctx.status(400);
                    response.addProperty("error", "Cannot manually set UNCONNECTED mode");
                    ctx.contentType("application/json");
                    ctx.result(gson.toJson(response));
                    return;
                }

                state.setMode(requestedMode);

                if (requestedMode == SystemMode.MANUAL) {
                    serial.sendModeManual();
                } else {
                    serial.sendModeAuto();
                }

                response.addProperty("success", true);
                response.addProperty("mode", requestedMode.getLabel());
                ctx.contentType("application/json");
                ctx.result(gson.toJson(response));

            } catch (IllegalArgumentException e) {
                ctx.status(400);
                response.addProperty("error", "Invalid mode: " + modeStr);
                ctx.contentType("application/json");
                ctx.result(gson.toJson(response));
            }
        });

        // POST /api/valve
        app.post("/api/valve", ctx -> {
            JsonObject response = new JsonObject();

            if (state.getMode() != SystemMode.MANUAL) {
                ctx.status(400);
                response.addProperty("error",
                    "Valve can only be set in MANUAL mode. Current: " + state.getMode().getLabel());
                ctx.contentType("application/json");
                ctx.result(gson.toJson(response));
                return;
            }

            JsonObject body = gson.fromJson(ctx.body(), JsonObject.class);
            int percent = body.get("percent").getAsInt();

            if (percent < 0 || percent > 100) {
                ctx.status(400);
                response.addProperty("error", "Percent must be 0-100. Got: " + percent);
                ctx.contentType("application/json");
                ctx.result(gson.toJson(response));
                return;
            }

            state.setValveOpenPercent(percent);
            serial.sendValveOpen(percent);

            response.addProperty("success", true);
            response.addProperty("valveOpenPercent", percent);
            ctx.contentType("application/json");
            ctx.result(gson.toJson(response));
        });
    }

    public void stop() {
        if (app != null) {
            app.stop();
        }
    }
}
