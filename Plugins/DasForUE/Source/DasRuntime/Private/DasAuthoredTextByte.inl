
void PublishFrameByte(int32 I, uint8 X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 17;
    V.ByteValue = X;
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameText(int32 I, FDasTextValue X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 18;
    V.TextValue = ResolveDSLText(X, C);
    PublishFrameValue(I, MoveTemp(V), C);
}
