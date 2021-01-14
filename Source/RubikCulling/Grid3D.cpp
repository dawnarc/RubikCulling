// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid3D.h"

#include "DrawDebugHelpers.h"

AGrid3D::AGrid3D()
{
	
}

void AGrid3D::SetBound(const FBox& InBound)
{
	Bound = InBound;
}

void AGrid3D::SetCellSize(float InCellSize)
{
	CellSize = InCellSize;
}

void AGrid3D::HandleActorOutOfSpatialBounds(const FVector& Location3D)
{
	if (SpatialBias.X > Location3D.X)
	{
		SpatialBias.X = Location3D.X - (CellSize / 2.f);
	}
	if (SpatialBias.Y > Location3D.Y)
	{
		SpatialBias.Y = Location3D.Y - (CellSize / 2.f);
	}
	if (SpatialBias.Z > Location3D.Z)
	{
		SpatialBias.Z = Location3D.Z - (CellSize / 2.f);
	}
}

TArray<TArray<UCellNode*>>& AGrid3D::GetGridX(int32 X)
{
	if (Cells.Num() <= X)
	{
		Cells.SetNum(X + 1);
	}
	return Cells[X];
}

TArray<UCellNode*>& AGrid3D::GetGridY(TArray<TArray<UCellNode*>>& GridX, int32 Y)
{
	if (GridX.Num() <= Y)
	{
		GridX.SetNum(Y + 1);
	}
	return GridX[Y];
}

UCellNode*& AGrid3D::GetCell(TArray<UCellNode*>& GridY, int32 Z)
{
	if (GridY.Num() <= Z)
	{
		GridY.SetNum(Z + 1);
	}
	return GridY[Z];
}

FCellInfo AGrid3D::GetCellInfoForActor(const FVector& TargetLocation, float InCullDistance, float InCellSize, const FVector& InSpatialBias)
{
	FVector ClampedLocation = TargetLocation;

	// Sanity check the actor's location. If it's garbage, we could end up with a gigantic allocation in GetGridNodesForActor as we adjust the grid.
	if (TargetLocation.X < -HALF_WORLD_MAX || TargetLocation.X > HALF_WORLD_MAX ||
        TargetLocation.Y < -HALF_WORLD_MAX || TargetLocation.Y > HALF_WORLD_MAX ||
        TargetLocation.Z < -HALF_WORLD_MAX || TargetLocation.Z > HALF_WORLD_MAX)
	{
		ClampedLocation = TargetLocation.BoundToCube(HALF_WORLD_MAX);
	}

	FCellInfo CellInfo;
	const float LocationBiasX = (ClampedLocation.X - InSpatialBias.X);
	const float LocationBiasY = (ClampedLocation.Y - InSpatialBias.Y);
	const float LocationBiasZ = (ClampedLocation.Z - InSpatialBias.Z);

	const float Dist = InCullDistance;
	const float MinX = LocationBiasX - Dist;
	const float MinY = LocationBiasY - Dist;
	const float MinZ = LocationBiasZ - Dist;
	float MaxX = LocationBiasX + Dist;
	float MaxY = LocationBiasY + Dist;
	float MaxZ = LocationBiasZ + Dist;

	CellInfo.StartX = FMath::Max<int32>(0, MinX / InCellSize);
	CellInfo.StartY = FMath::Max<int32>(0, MinY / InCellSize);
	CellInfo.StartZ = FMath::Max<int32>(0, MinZ / InCellSize);

	CellInfo.EndX = FMath::Max<int32>(0, MaxX / InCellSize);
	CellInfo.EndY = FMath::Max<int32>(0, MaxY / InCellSize);
	CellInfo.EndZ = FMath::Max<int32>(0, MaxZ / InCellSize);
	
	return CellInfo;
}

void AGrid3D::DrawDebugCellInfo(const FVector& TargetLocation, float InCullDistance, float InCellSize, const FVector& InSpatialBias)
{
	FCellInfo NewCellInfo = GetCellInfoForActor(TargetLocation, InCullDistance, InCellSize, InSpatialBias);
	for(int32 X = NewCellInfo.StartX; X <= NewCellInfo.EndX; X++)
	{
		for(int32 Y = NewCellInfo.StartY; Y <= NewCellInfo.EndY; Y++)
		{
			for(int32 Z = NewCellInfo.StartZ; Z <= NewCellInfo.EndZ; Z++)
			{
				FVector TargetCell;
				TargetCell.X = X * InCellSize + InSpatialBias.X;
				TargetCell.Y = Y * InCellSize + InSpatialBias.Y;
				TargetCell.Z = Z * InCellSize + InSpatialBias.Z;
				DrawDebugBox(GetWorld(), TargetCell + InCellSize / 2, FVector(InCellSize, InCellSize, InCellSize),
					FColor::Yellow, false, 0.1, 0, 1.f);
			}
		}	
	}
}

void AGrid3D::Rebuild(const FVector& TargetLocation)
{
	FCellInfo NewCellInfo = GetCellInfoForActor(TargetLocation, CullDistance, CellSize, SpatialBias);

	if(PrevCellInfo.IsValid())
	{
		if (NewCellInfo.StartX > PrevCellInfo.EndX || NewCellInfo.EndX < PrevCellInfo.StartX ||
                             NewCellInfo.StartY > PrevCellInfo.EndY || NewCellInfo.EndY < PrevCellInfo.StartY ||
                             NewCellInfo.StartZ > PrevCellInfo.EndZ || NewCellInfo.EndZ < PrevCellInfo.StartZ)
		{
			for(int32 x = PrevCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
			{
				TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
				for(int32 y = PrevCellInfo.StartY; y <= PrevCellInfo.EndY; y++)
				{
					TArray<UCellNode*>& GridY = GetGridY(GridX, y);
					for(int32 z = PrevCellInfo.StartZ; z <= PrevCellInfo.EndZ; z++)
					{
						UCellNode* Cell = GetCellNode(GetCell(GridY, z));
						Cell->DrawRed(x, y, z, CellSize, SpatialBias);
					}
				}
			}

			for(int32 x = NewCellInfo.StartX; x <= NewCellInfo.EndX; x++)
			{
				TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
				for(int32 y = NewCellInfo.StartY; y <= NewCellInfo.EndY; y++)
				{
					TArray<UCellNode*>& GridY = GetGridY(GridX, y);
					for(int32 z = NewCellInfo.StartZ; z <= NewCellInfo.EndZ; z++)
					{
						UCellNode* Cell = GetCellNode(GetCell(GridY, z));
						Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
					}
				}
			}
		}
		else
		{
			// ================== calculate cells along X axis ================== 
			if(PrevCellInfo.StartX < NewCellInfo.StartX)
			{
				//remove
				for(int32 x = PrevCellInfo.StartX; x < NewCellInfo.StartX; x++)
				{
					TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
					for(int32 y = PrevCellInfo.StartY; y < PrevCellInfo.EndY; y++)
					{
						TArray<UCellNode*>& GridY = GetGridY(GridX, y);
						for(int32 z = PrevCellInfo.StartZ; z < PrevCellInfo.EndZ; z++)
						{
							UCellNode* Cell = GetCellNode(GetCell(GridY, z));
							Cell->DrawRed(x, y, z, CellSize, SpatialBias);
						}
					}
				}

				//add
				for(int32 x = PrevCellInfo.EndX + 1; x <= NewCellInfo.EndX; x++)
				{
					TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
					for(int32 y = NewCellInfo.StartY; y <= NewCellInfo.EndY; y++)
					{
						TArray<UCellNode*>& GridY = GetGridY(GridX, y);
						for(int32 z = NewCellInfo.StartZ; z <= NewCellInfo.EndZ; z++)
						{
							UCellNode* Cell = GetCellNode(GetCell(GridY, z));
							Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
						}
					}
				}
			}
			else if(PrevCellInfo.StartX > NewCellInfo.StartX)
			{
				//remove
				for(int32 x = NewCellInfo.EndX + 1; x <= PrevCellInfo.EndX; x++)
				{
					TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
					for(int32 y = PrevCellInfo.StartY; y < PrevCellInfo.EndY; y++)
					{
						TArray<UCellNode*>& GridY = GetGridY(GridX, y);
						for(int32 z = PrevCellInfo.StartZ; z < PrevCellInfo.EndZ; z++)
						{
							UCellNode* Cell = GetCellNode(GetCell(GridY, z));
							Cell->DrawRed(x, y, z, CellSize, SpatialBias);
							//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
						}
					}
				}

				//add
				for(int32 x = NewCellInfo.StartX; x < PrevCellInfo.StartX; x++)
				{
					TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
					for(int32 y = NewCellInfo.StartY; y <= NewCellInfo.EndY; y++)
					{
						TArray<UCellNode*>& GridY = GetGridY(GridX, y);
						for(int32 z = NewCellInfo.StartZ; z <= NewCellInfo.EndZ; z++)
						{
							UCellNode* Cell = GetCellNode(GetCell(GridY, z));
							Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
							//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
						}
					}
				}
			}

			// ================== calculate cells along Y axis ================== 
			if(PrevCellInfo.StartY < NewCellInfo.StartY)
			{
				if(PrevCellInfo.StartX < NewCellInfo.StartX)
				{
					//remove
					for(int32 x = NewCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
					{
						TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
						for(int32 y = PrevCellInfo.StartY; y < NewCellInfo.StartY; y++)
						{
							TArray<UCellNode*>& GridY = GetGridY(GridX, y);
							for(int32 z = PrevCellInfo.StartZ; z <= PrevCellInfo.EndZ; z++)
							{
								UCellNode* Cell = GetCellNode(GetCell(GridY, z));
								Cell->DrawRed(x, y, z, CellSize, SpatialBias);
								//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
							}
						}
					}
				
					//add
					for(int32 x = NewCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
					{
						TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
						for(int32 y = PrevCellInfo.EndY + 1; y <= NewCellInfo.EndY; y++)
						{
							TArray<UCellNode*>& GridY = GetGridY(GridX, y);
							for(int32 z = NewCellInfo.StartZ; z <= NewCellInfo.EndZ; z++)
							{
								UCellNode* Cell = GetCellNode(GetCell(GridY, z));
								Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
								//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
							}
						}
					}
				}
				else if(PrevCellInfo.StartX > NewCellInfo.StartX)
				{
					//remove
					for(int32 x = PrevCellInfo.StartX; x <= NewCellInfo.EndX; x++)
					{
						TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
						for(int32 y = PrevCellInfo.StartY; y < NewCellInfo.StartY; y++)
						{
							TArray<UCellNode*>& GridY = GetGridY(GridX, y);
							for(int32 z = PrevCellInfo.StartZ; z <= PrevCellInfo.EndZ; z++)
							{
								UCellNode* Cell = GetCellNode(GetCell(GridY, z));
								Cell->DrawRed(x, y, z, CellSize, SpatialBias);
								//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
							}
						}
					}

					//add
					for(int32 x = PrevCellInfo.StartX; x <= NewCellInfo.EndX; x++)
					{
						TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
						for(int32 y = PrevCellInfo.EndY + 1; y <= NewCellInfo.EndY; y++)
						{
							TArray<UCellNode*>& GridY = GetGridY(GridX, y);
							for(int32 z = NewCellInfo.StartZ; z <= NewCellInfo.EndZ; z++)
							{
								UCellNode* Cell = GetCellNode(GetCell(GridY, z));
								Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
								//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
							}
						}
					}
				}
			}
			else if(PrevCellInfo.StartY > NewCellInfo.StartY)
			{
				if(PrevCellInfo.StartX < NewCellInfo.StartX)
				{
					//remove
					for(int32 x = NewCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
					{
						TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
						for(int32 y = NewCellInfo.EndY + 1; y <= PrevCellInfo.EndY; y++)
						{
							TArray<UCellNode*>& GridY = GetGridY(GridX, y);
							for(int32 z = PrevCellInfo.StartZ; z <= PrevCellInfo.EndZ; z++)
							{
								UCellNode* Cell = GetCellNode(GetCell(GridY, z));
								Cell->DrawRed(x, y, z, CellSize, SpatialBias);
								//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
							}
						}
					}

					//add
					for(int32 x = NewCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
					{
						TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
						for(int32 y = NewCellInfo.StartY; y < PrevCellInfo.StartY; y++)
						{
							TArray<UCellNode*>& GridY = GetGridY(GridX, y);
							for(int32 z = NewCellInfo.StartZ; z <= NewCellInfo.EndZ; z++)
							{
								UCellNode* Cell = GetCellNode(GetCell(GridY, z));
								Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
								//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
							}
						}
					}
				}
				else if(PrevCellInfo.StartX > NewCellInfo.StartX)
				{
					//remove
					for(int32 x = PrevCellInfo.StartX; x <= NewCellInfo.EndX; x++)
					{
						TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
						for(int32 y = NewCellInfo.EndY + 1; y <= PrevCellInfo.EndY; y++)
						{
							TArray<UCellNode*>& GridY = GetGridY(GridX, y);
							for(int32 z = PrevCellInfo.StartZ; z <= PrevCellInfo.EndZ; z++)
							{
								UCellNode* Cell = GetCellNode(GetCell(GridY, z));
								Cell->DrawRed(x, y, z, CellSize, SpatialBias);
								//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
							}
						}
					}

					//add
					for(int32 x = PrevCellInfo.StartX; x <= NewCellInfo.EndX; x++)
					{
						TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
						for(int32 y = NewCellInfo.StartY; y < PrevCellInfo.StartY; y++)
						{
							TArray<UCellNode*>& GridY = GetGridY(GridX, y);
							for(int32 z = NewCellInfo.StartZ; z <= NewCellInfo.EndZ; z++)
							{
								UCellNode* Cell = GetCellNode(GetCell(GridY, z));
								Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
								//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
							}
						}
					}
				}
			}

			// ================== calculate cells along Z axis ==================
			if(PrevCellInfo.StartZ < NewCellInfo.StartZ)
			{
				if(PrevCellInfo.StartY < NewCellInfo.StartY)
				{
					if(PrevCellInfo.StartX < NewCellInfo.StartX)
					{
						//remove
						for(int32 x = NewCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = NewCellInfo.StartY; y <= PrevCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = PrevCellInfo.StartZ; z < NewCellInfo.StartZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawRed(x, y, z, CellSize, SpatialBias);
									//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}

						//add
						for(int32 x = NewCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = NewCellInfo.StartY; y <= PrevCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = PrevCellInfo.EndZ + 1; z <= NewCellInfo.EndZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
									//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}
					}
					else if(PrevCellInfo.StartX > NewCellInfo.StartX)
					{
						//remove
						for(int32 x = PrevCellInfo.StartX; x <= NewCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = NewCellInfo.StartY; y <= PrevCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = PrevCellInfo.StartZ; z < NewCellInfo.StartZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawRed(x, y, z, CellSize, SpatialBias);
									//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}

						//add
						for(int32 x = PrevCellInfo.StartX; x <= NewCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = NewCellInfo.StartY; y <= PrevCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = PrevCellInfo.EndZ + 1; z <= NewCellInfo.EndZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
									//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}
					}
				}
				else if(PrevCellInfo.StartY > NewCellInfo.StartY)
				{
					if(PrevCellInfo.StartX < NewCellInfo.StartX)
					{
						//remove
						for(int32 x = NewCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = PrevCellInfo.StartY; y <= NewCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = PrevCellInfo.StartZ; z < NewCellInfo.StartZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawRed(x, y, z, CellSize, SpatialBias);
									//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}

						//add
						for(int32 x = NewCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = PrevCellInfo.StartY; y <= NewCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = PrevCellInfo.EndZ + 1; z <= NewCellInfo.EndZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
									//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}
					}
					else if(PrevCellInfo.StartX > NewCellInfo.StartX)
					{
						//remove
						for(int32 x = PrevCellInfo.StartX; x <= NewCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = PrevCellInfo.StartY; y <= NewCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = PrevCellInfo.StartZ; z < NewCellInfo.StartZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawRed(x, y, z, CellSize, SpatialBias);
									//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}

						//add
						for(int32 x = PrevCellInfo.StartX; x <= NewCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = PrevCellInfo.StartY; y <NewCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = PrevCellInfo.EndZ + 1; z <= NewCellInfo.EndZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
									//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}
					}
				}
			}
			else if(PrevCellInfo.StartZ > NewCellInfo.StartZ)
			{
				if(PrevCellInfo.StartY < NewCellInfo.StartY)
				{
					if(PrevCellInfo.StartX < NewCellInfo.StartX)
					{
						//remove
						for(int32 x = NewCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = NewCellInfo.StartY; y <= PrevCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = NewCellInfo.EndZ + 1; z <= PrevCellInfo.EndZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawRed(x, y, z, CellSize, SpatialBias);
									//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}

						//add
						for(int32 x = NewCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = NewCellInfo.StartY; y <= PrevCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = NewCellInfo.StartZ; z < PrevCellInfo.StartZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
									//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}
					}
					else if(PrevCellInfo.StartX > NewCellInfo.StartX)
					{
						//remove
						for(int32 x = PrevCellInfo.StartX; x <= NewCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = NewCellInfo.StartY; y <= PrevCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = NewCellInfo.EndZ + 1; z <= PrevCellInfo.EndZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawRed(x, y, z, CellSize, SpatialBias);
									//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}

						//add
						for(int32 x = PrevCellInfo.StartX; x <= NewCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = NewCellInfo.StartY; y <= PrevCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = NewCellInfo.StartZ; z < PrevCellInfo.StartZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
									//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}
					}
				}
				else if(PrevCellInfo.StartY > NewCellInfo.StartY)
				{
					if(PrevCellInfo.StartX < NewCellInfo.StartX)
					{
						//remove
						for(int32 x = NewCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = PrevCellInfo.StartY; y <= NewCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = NewCellInfo.EndZ + 1; z <= PrevCellInfo.EndZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawRed(x, y, z, CellSize, SpatialBias);
									//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}

						//add
						for(int32 x = NewCellInfo.StartX; x <= PrevCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = PrevCellInfo.StartY; y <= NewCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = NewCellInfo.StartZ; z < PrevCellInfo.StartZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
									//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}
					}
					else if(PrevCellInfo.StartX > NewCellInfo.StartX)
					{
						//remove
						for(int32 x = PrevCellInfo.StartX; x <= NewCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = PrevCellInfo.StartY; y <= NewCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = NewCellInfo.EndZ + 1; z <= PrevCellInfo.EndZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawRed(x, y, z, CellSize, SpatialBias);
									//ToBeHiddenActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}

						//add
						for(int32 x = PrevCellInfo.StartX; x <= NewCellInfo.EndX; x++)
						{
							TArray<TArray<UCellNode*>>& GridX = GetGridX(x);
							for(int32 y = PrevCellInfo.StartY; y <= NewCellInfo.EndY; y++)
							{
								TArray<UCellNode*>& GridY = GetGridY(GridX, y);
								for(int32 z = NewCellInfo.StartZ; z < PrevCellInfo.StartZ; z++)
								{
									UCellNode* Cell = GetCellNode(GetCell(GridY, z));
									Cell->DrawGreen(x, y, z, CellSize, SpatialBias);
									//ToBeVisibleActors.Append(GetDistCullCell(GetUniqueCell(x, y, z))->GetActors());
								}
							}
						}
					}
				}
			}
		}
	}

	PrevCellInfo = NewCellInfo;
}

void AGrid3D::Tick(float DeltaTime)
{
	
}
