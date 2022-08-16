block is set from string
then gcode parses block to convert into a simplifiedMotion or parseError


TODO
    // If we are in inverse feedmode or G91, we should reset F and axis for each block
    // even going to these modes in a block, we should do that.. before reading any axis words
    // TODO : put some extra tests in the code, depending on the number of Axis
    // If 1 axis -> no helical possible
    // if 2 axis -> only 1 plane possible
    // speed optimization : put code in blocks to only execute if there are still gCode words - most lines will contain only axis words, and there is no need to search for all possible options if there ar eno more words..
    // even if this search returns immediately when nmbrWord == 0
    // G0 or G1 without axis : could be error or just set modal group motion..
