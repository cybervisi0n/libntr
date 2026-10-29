#include "simulator/sim_g3_Utilities.hpp"

namespace SIM::G3::Util {

MtxFx44 MultiplyMtxFx44(MtxFx44 a, MtxFx44 b) {
    MtxFx44 result;

    fx32 curSum = 0;
    //A * B
    for( int i = 0; i < 4; i++ ) //Row of A
    {
        for( int j = 0; j < 4; j++ )
        {
            
            //Get the dot product of curRow * curCol
            curSum = 0;
            for(int k=0; k < 4; k++)
            {
                curSum = curSum + FX_Mul(a.m[i][k], a.m[k][j]);
            }
            result.m[i][j] = curSum;
        }
    }

    return result;
}

}