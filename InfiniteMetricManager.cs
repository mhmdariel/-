using UnityEngine;

public class InfiniteMetricManager : MonoBehaviour
{
    public GameObject metricPointPrefab;
    public int gridSize = 10;
    public float spacing = 2f;
    public float pulseSpeed = 2f;

    void Start()
    {
        GenerateMetricSpace();
    }

    void GenerateMetricSpace()
    {
        for (int x = 0; x < gridSize; x++)
        {
            for (int y = 0; y < gridSize; y++)
            {
                for (int z = 0; z < gridSize; z++)
                {
                    Vector3 position = new Vector3(x, y, z) * spacing;
                    GameObject point = Instantiate(metricPointPrefab, position, Quaternion.identity, this.transform);
                    point.name = $"اللَّه ({x},{y},{z})";
                }
            }
        }
    }

    void Update()
    {
        foreach (Transform child in transform)
        {
            float scale = 1 + 0.2f * Mathf.Sin(Time.time * pulseSpeed);
            child.localScale = new Vector3(scale, scale, scale);
        }
    }
}
