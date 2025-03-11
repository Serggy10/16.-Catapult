#ifndef _ACCIONADOR_H_
#define _ACCIONADOR_H_

#include <SinricProDevice.h>
#include <Capabilities/ToggleController.h>
#include <Capabilities/ModeController.h>

class Accionador 
: public SinricProDevice
, public ToggleController<Accionador>
, public ModeController<Accionador> {
  friend class ToggleController<Accionador>;
  friend class ModeController<Accionador>;
public:
  Accionador(const String &deviceId) : SinricProDevice(deviceId, "Accionador") {};
};

#endif
