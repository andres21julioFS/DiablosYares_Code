# DiablosYares_Code
En este repositorio se guardaran muestras de código desarrollado por Andrés Fernández para el videojuego Diablos Yares.
Breve resumen de cada clase:
  -PhysicsMotion: Mueve al personaje utilizando fuerzas. Así entonces el personaje podrá ser empujado por otros objetos. Este componente funciona como una caja negra y es independiente del tipo de personaje.
  -Grabber: Permite agarrar y mover objetos por el escenario a través del motor de físicas. Para ello también se utiliza un componente de Unreal llamado PhysicsHandle.
  -BTTask_PhysicMoveTo: Tarea que podrá ser utilizada por un Behaviour Tree. El objetivo es hacer que los bots se muevan a través de fuerzas. Básicamente indica al controlador de un bot a donde tiene que dirigirse. 
  El componente AIPhysicMoveTo se encargará de mover al personaje.
  -AIPhysicMoveTo: Busca una ruta al destino (a través de UNavigationSystemV1::FindPathToLocationSynchronously) para obtener la direción que debe tomar el bot. A continuación se utilizara el componente PhysicsMotion.
