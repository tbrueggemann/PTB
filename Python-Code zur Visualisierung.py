import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from skspatial.objects import Plane, Points

# Hier fügst du den gesamten Inhalt der txt-Datei ein (kopiere und ersetze den Text in triple quotes)
depth_data_text = """ Tiefenwerte... """
#
# Umwandlung des txt-Inhalts in eine Liste von Listen (2D-Array)
depth_data = [list(map(int, line.split())) for line in depth_data_text.strip().splitlines()]

# Konvertierung der Tiefendaten in ein NumPy-Array
depth_array = np.array(depth_data)

# Erstellen eines Meshgrids für die X- und Y-Koordinaten
x = np.arange(depth_array.shape[1])
y = np.arange(depth_array.shape[0])
x, y = np.meshgrid(x, y)

# Erstelle eine 3D-Ansicht
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Zeichnen der Oberfläche
ax.plot_surface(x, y, depth_array, cmap='viridis', alpha=0.7)

# Achsenbeschriftungen
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Depth')

# Zeige die Legende
ax.legend()

# Anzeigen der 3D-Ansicht
plt.show()