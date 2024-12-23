#include <librealsense2/rs.hpp>
#include <librealsense2/rs_advanced_mode.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <unistd.h>

int main() {
    try {
        rs2::pipeline pipe;
        rs2::config cfg;

        // Pipeline starten
        rs2::pipeline_profile profile = pipe.start(cfg);
        rs2::device selected_device = profile.get_device();

        // Preset laden, sofern möglich
        if (selected_device.is<rs400::advanced_mode>()) {
            auto advanced_mode_dev = selected_device.as<rs400::advanced_mode>();

            std::ifstream file("/home/eggpi/scripts/DepthCorrection/ShortRangePreset.json");
            if (!file.is_open()) {
                std::cerr << "Fehler beim Öffnen der Preset-Datei: /home/eggpi/scripts/DepthCorrection/ShortRangePreset.json" << std::endl;
                return -1;
            }

            std::string json_string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            if (advanced_mode_dev.is_enabled()) {
                std::cout << "Advanced Mode ist aktiviert, lade Einstellungen aus der JSON-Datei." << std::endl;
                advanced_mode_dev.load_json(json_string);
            } else {
                std::cerr << "Advanced Mode ist nicht aktiviert." << std::endl;
                return -1;
            }
        } else {
            std::cerr << "Advanced Mode wird nicht unterstützt." << std::endl;
            return -1;
        }

        // Pipeline neu konfigurieren mit Depth-Stream
        pipe.stop();
        cfg.enable_stream(RS2_STREAM_DEPTH, 848, 480, RS2_FORMAT_Z16, 30);
        pipe.start(cfg);

        // Kurze Pause, um sicherzugehen, dass der Stream läuft
        sleep(2);

        // Einen Frameset holen
        rs2::frameset frameset = pipe.wait_for_frames();
        rs2::frame depth_frame = frameset.get_depth_frame();

        int width = depth_frame.as<rs2::video_frame>().get_width();
        int height = depth_frame.as<rs2::video_frame>().get_height();
        const uint16_t* depth_data = reinterpret_cast<const uint16_t*>(depth_frame.get_data());

        // Ausgabedatei öffnen
        std::ofstream output_file("/var/www/html/3d_plane.txt");
        if (!output_file.is_open()) {
            std::cerr << "Fehler beim Öffnen der Ausgabedatei: /var/www/html/3d_plane.txt" << std::endl;
            return -1;
        }

        // Alle Pixelwerte ausgeben, dabei auf max. 2000 mm begrenzen
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                uint16_t depth_value = depth_data[y * width + x];

                // Werte oberhalb von 2m auf 0 setzen
                if (depth_value > 2000) {
                    depth_value = 0;
                }

                output_file << depth_value;
                if (x < width - 1) output_file << " ";
            }
            output_file << "\n"; // Neue Zeile nach jedem Bild-Row
        }

        output_file.close();
        std::cout << "Tiefenwerte wurden erfolgreich gespeichert (0-2m begrenzt)." << std::endl;

        pipe.stop();
    } catch (const rs2::error &e) {
        std::cerr << "RealSense error calling " << e.get_failed_function()
                  << "(" << e.get_failed_args() << "): " << e.what() << std::endl;
        return -1;
    } catch (const std::exception &e) {
        std::cerr << "Allgemeiner Fehler: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
