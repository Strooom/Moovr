Events can be handled in two ways :
1. When they can only occur occasionally, (eg gcode syntax error), they are sent on the eventBuffer and the mainController handles them
2. When they can occur in a 'flood' (eg buffer underflow/overflow errors) then the module only logs the lastError internally, and the mainController polls the module for any errors, so it can never be flooded with the same error