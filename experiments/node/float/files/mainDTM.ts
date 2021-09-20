export module main
{
class Point
{
	x : number;
	y : number;
	z : number;
	constructor(i : number)
	{
		this.x = Math.sin(i);
		this.y = Math.cos(i) * 3;
		this.z = (this.x * this.x) / 2;
	}
	
	Normalize() : void
	{
		var norm : number = Math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z);
		this.x = this.x / norm;
		this.y = this.y / norm;
		this.z = this.z / norm;
	}
	
	Maximize(other : Point) : Point
	{
		if(this.x < other.x)
		{
			this.x = other.x;
		}
		if(this.y < other.y)
		{
			this.y = other.y;
		}
		if(this.z < other.z)
		{
			this.z = other.z;
		}
		return this;
	}
	
	Print():void
	{
		console.log("<"+this.x+","+this.y+","+this.z+">");
	}
}

function Maximize(points : Array<Point>) : Point
	{
		var next : Point = points[0];
		for(let i : number = 1;i < points.length;i++)
		{
			var p = points[i];
			next = next.Maximize(p);
		}
		return next;
	}
function  Benchmark(n : number) : Point
	{
		var points : Array<Point> = Array(n);
		for(let i : number = 0;i < points.length;i++)
		{
		  points[i]=new Point(i);
		}
		for(let i : number = 0;i < points.length;i++)
		{
			var p = points[i];
			p.Normalize();
		}
		return Maximize(points);
	}
export function main()
	{
		Benchmark(100000);
	}
}